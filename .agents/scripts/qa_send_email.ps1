<#
.SYNOPSIS
    QA/검토 결과 보고서를 Gmail로 전송합니다.
    우선: Gmail REST API (OAuth2 토큰), 폴백: .NET SmtpClient (앱 비밀번호)

.PARAMETER To
    수신자 이메일 주소 (기본: plaied45@gmail.com)

.PARAMETER From
    발신자 Gmail 주소

.PARAMETER Subject
    이메일 제목

.PARAMETER BodyFile
    본문으로 사용할 파일 (.md/.txt). 없으면 기본 메시지 사용.

.PARAMETER Attachment
    첨부파일 경로 (선택)

.PARAMETER GmailAppPassword
    SMTP 폴백용 앱 비밀번호 (환경변수 GMAIL_APP_PASSWORD 가능)

.PARAMETER TokenPath
    OAuth2 토큰 파일 경로 (기본: .agents/secrets/gmail_token.json)

.EXAMPLE
    # OAuth2 방식 (권장 — 토큰 파일만 있으면 자동)
    powershell -ExecutionPolicy Bypass -File .\.agents\scripts\qa_send_email.ps1 `
        -From "yourname@gmail.com" `
        -Subject "[QA] FbxViewer Report" `
        -BodyFile "qa_report.md"

    # SMTP 폴백 방식 (앱 비밀번호 사용)
    $env:GMAIL_APP_PASSWORD = "xxxx xxxx xxxx xxxx"
    powershell -ExecutionPolicy Bypass -File .\.agents\scripts\qa_send_email.ps1 `
        -From "yourname@gmail.com" `
        -Subject "[QA] FbxViewer Report" `
        -BodyFile "qa_report.md"
#>
param (
    [string]$To = "plaied45@gmail.com",
    [string]$From = "",
    [string]$Subject = "QA Report: FbxViewer",
    [string]$BodyFile = "",
    [string]$Attachment = "",
    [string]$GmailAppPassword = "", # PSScriptAnalyzer suppress PSAvoidUsingPlainTextForPassword — 환경변수 폴백 지원 필요
    [string]$TokenPath = ".\.agents\secrets\gmail_token.json"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# ═══════════════════════════════════════════════════════════════════════════════
# 공통 유틸리티
# ═══════════════════════════════════════════════════════════════════════════════

function Get-EmailBody {
    $body = "QA 검증이 완료되었습니다. 첨부 파일을 확인하세요."
    if ($BodyFile -and (Test-Path $BodyFile)) {
        $body = Get-Content $BodyFile -Raw -Encoding UTF8
    }
    elseif ($BodyFile) {
        Write-Warning "[WARN] 본문 파일을 찾을 수 없습니다: $BodyFile — 기본 메시지를 사용합니다."
    }
    return $body
}

function Show-SendInfo {
    Write-Output ""
    Write-Output "이메일 전송 준비 중..."
    Write-Output "  수신자 : $To"
    Write-Output "  발신자 : $From"
    Write-Output "  제목   : $Subject"
    Write-Output "  첨부   : $(if ($Attachment -and (Test-Path $Attachment)) { $Attachment } else { '없음' })"
}

# ═══════════════════════════════════════════════════════════════════════════════
# 방법 A: Gmail REST API (OAuth2)
# ═══════════════════════════════════════════════════════════════════════════════

function Update-AccessToken {
    param($tokenData)

    $refreshBody = @{
        client_id     = $tokenData.client_id
        client_secret = $tokenData.client_secret
        refresh_token = $tokenData.refresh_token
        grant_type    = "refresh_token"
    }

    $response = Invoke-RestMethod -Uri "https://oauth2.googleapis.com/token" `
        -Method Post `
        -ContentType "application/x-www-form-urlencoded" `
        -Body $refreshBody

    return $response.access_token
}

function ConvertTo-Base64Url {
    param([byte[]]$bytes)
    $base64 = [Convert]::ToBase64String($bytes)
    return $base64.Replace("+", "-").Replace("/", "_").TrimEnd("=")
}

function New-RawEmail {
    param(
        [string]$fromAddr,
        [string]$toAddr,
        [string]$subjectText,
        [string]$bodyText,
        [string]$attachmentPath
    )

    $boundary = "boundary_$(New-Guid)"

    $headers = "From: $fromAddr`r`n"
    $headers += "To: $toAddr`r`n"
    $headers += "Subject: =?UTF-8?B?$([Convert]::ToBase64String([System.Text.Encoding]::UTF8.GetBytes($subjectText)))?=`r`n"
    $headers += "MIME-Version: 1.0`r`n"

    if ($attachmentPath -and (Test-Path $attachmentPath)) {
        # Multipart 이메일
        $headers += "Content-Type: multipart/mixed; boundary=`"$boundary`"`r`n`r`n"

        $textPart = "--$boundary`r`n"
        $textPart += "Content-Type: text/plain; charset=UTF-8`r`n"
        $textPart += "Content-Transfer-Encoding: base64`r`n`r`n"
        $textPart += [Convert]::ToBase64String([System.Text.Encoding]::UTF8.GetBytes($bodyText))
        $textPart += "`r`n"

        $fileName = [System.IO.Path]::GetFileName($attachmentPath)
        $fileBytes = [System.IO.File]::ReadAllBytes($attachmentPath)
        $fileBase64 = [Convert]::ToBase64String($fileBytes)

        $attachPart = "--$boundary`r`n"
        $attachPart += "Content-Type: application/octet-stream; name=`"$fileName`"`r`n"
        $attachPart += "Content-Disposition: attachment; filename=`"$fileName`"`r`n"
        $attachPart += "Content-Transfer-Encoding: base64`r`n`r`n"
        $attachPart += $fileBase64
        $attachPart += "`r`n--$boundary--"

        $rawMessage = $headers + $textPart + $attachPart
    }
    else {
        # 단순 텍스트 이메일
        $headers += "Content-Type: text/plain; charset=UTF-8`r`n"
        $headers += "Content-Transfer-Encoding: base64`r`n`r`n"
        $rawMessage = $headers + [Convert]::ToBase64String([System.Text.Encoding]::UTF8.GetBytes($bodyText))
    }

    return $rawMessage
}

function Send-ViaGmailApi {
    Write-Output "[방법] Gmail REST API (OAuth2)"

    $tokenData = Get-Content $TokenPath -Raw -Encoding UTF8 | ConvertFrom-Json

    if (-not $tokenData.refresh_token) {
        throw "토큰 파일에 refresh_token이 없습니다. gmail_oauth_setup.ps1을 다시 실행하세요."
    }

    # 액세스 토큰 갱신
    Write-Output "  액세스 토큰 갱신 중..."
    $accessToken = Update-AccessToken -tokenData $tokenData

    # 이메일 MIME 구성
    $bodyText = Get-EmailBody
    $rawMessage = New-RawEmail -fromAddr $From -toAddr $To `
        -subjectText $Subject -bodyText $bodyText `
        -attachmentPath $Attachment

    $rawBytes = [System.Text.Encoding]::UTF8.GetBytes($rawMessage)
    $encodedMsg = ConvertTo-Base64Url -bytes $rawBytes

    $requestBody = @{ raw = $encodedMsg } | ConvertTo-Json

    # Gmail API 전송
    $apiHeaders = @{
        Authorization = "Bearer $accessToken"
    }

    Invoke-RestMethod -Uri "https://www.googleapis.com/gmail/v1/users/me/messages/send" `
        -Method Post `
        -Headers $apiHeaders `
        -ContentType "application/json; charset=UTF-8" `
        -Body $requestBody | Out-Null

    Write-Output "[SUCCESS] Gmail API로 이메일이 전송되었습니다 → $To"
}

# ═══════════════════════════════════════════════════════════════════════════════
# 방법 B: .NET SmtpClient (SMTP 폴백)
# ═══════════════════════════════════════════════════════════════════════════════

function Send-ViaSmtpClient {
    Write-Output "[방법] .NET SmtpClient (SMTP 폴백)"

    $bodyText = Get-EmailBody

    $mailMessage = New-Object System.Net.Mail.MailMessage
    $mailMessage.From = New-Object System.Net.Mail.MailAddress($From)
    $mailMessage.To.Add($To)
    $mailMessage.Subject = $Subject
    $mailMessage.Body = $bodyText
    $mailMessage.SubjectEncoding = [System.Text.Encoding]::UTF8
    $mailMessage.BodyEncoding = [System.Text.Encoding]::UTF8

    if ($Attachment -and (Test-Path $Attachment)) {
        $att = New-Object System.Net.Mail.Attachment($Attachment)
        $mailMessage.Attachments.Add($att)
    }
    elseif ($Attachment) {
        Write-Warning "[WARN] 첨부파일을 찾을 수 없습니다: $Attachment — 첨부 없이 전송합니다."
    }

    $smtpClient = New-Object System.Net.Mail.SmtpClient("smtp.gmail.com", 587)
    $smtpClient.EnableSsl = $true
    $smtpClient.Credentials = New-Object System.Net.NetworkCredential($From, $GmailAppPassword)

    try {
        $smtpClient.Send($mailMessage)
        Write-Output "[SUCCESS] SMTP로 이메일이 전송되었습니다 → $To"
    }
    finally {
        $mailMessage.Dispose()
        $smtpClient.Dispose()
    }
}

# ═══════════════════════════════════════════════════════════════════════════════
# 메인 실행 로직
# ═══════════════════════════════════════════════════════════════════════════════

# ── 발신자 확인 ───────────────────────────────────────────────────────────────
if (-not $From) {
    $From = $env:GMAIL_FROM_ADDRESS
}

if (-not $From) {
    Write-Error @"
[ERROR] -From parameter is missing. Please specify your Gmail address.

How to fix:
  1. Use command line: .\qa_send_email.ps1 -From "yourname@gmail.com"
  2. Use environment variable: `$env:GMAIL_FROM_ADDRESS = "yourname@gmail.com"
"@
    exit 1
}

Show-SendInfo

$useOAuth = Test-Path $TokenPath
$useSmtp = $false

if (-not $GmailAppPassword) {
    $GmailAppPassword = $env:GMAIL_APP_PASSWORD
}
if ($GmailAppPassword) {
    $useSmtp = $true
}

try {
    if ($useOAuth) {
        # OAuth2 우선 시도
        Send-ViaGmailApi
    }
    elseif ($useSmtp) {
        # SMTP 폴백
        Send-ViaSmtpClient
    }
    else {
        Write-Error @"
[ERROR] 이메일 전송을 위한 인증 정보가 없습니다.

=== 방법 1: Gmail API (OAuth2, 권장) ===
  1. gmail_oauth_setup.ps1 을 실행하여 인증 설정
     powershell -ExecutionPolicy Bypass -File .\.agents\scripts\gmail_oauth_setup.ps1

=== 방법 2: SMTP (앱 비밀번호) ===
  1. https://myaccount.google.com/apppasswords 에서 앱 비밀번호 생성
  2. 환경변수 설정: `$env:GMAIL_APP_PASSWORD = "xxxx xxxx xxxx xxxx"
     또는 -GmailAppPassword 파라미터 전달
"@
        exit 1
    }
}
catch {
    if ($useOAuth -and $useSmtp) {
        Write-Warning "[WARN] Gmail API 전송 실패: $_ — SMTP 폴백을 시도합니다."
        try {
            Send-ViaSmtpClient
        }
        catch {
            Write-Error "[ERROR] SMTP 폴백 전송도 실패: $_"
            exit 1
        }
    }
    else {
        Write-Error "[ERROR] 이메일 전송 실패: $_"
        exit 1
    }
}
