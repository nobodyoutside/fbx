<#
.SYNOPSIS
    Gmail REST API용 OAuth2 인증을 설정합니다. 최초 1회만 실행하면 됩니다.
    인증 완료 후 refresh_token을 .agents/secrets/gmail_token.json에 저장합니다.

.PARAMETER ClientSecretPath
    Google Cloud Console에서 다운로드한 client_secret.json 경로

.PARAMETER TokenPath
    토큰을 저장할 경로 (기본: .agents/secrets/gmail_token.json)

.PARAMETER Port
    로컬 리다이렉트용 포트 (기본: 8585)

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File .\.agents\scripts\gmail_oauth_setup.ps1 `
        -ClientSecretPath ".\.agents\secrets\client_secret.json"
#>
param (
    [string]$ClientSecretPath = ".\.agents\secrets\client_secret.json",
    [string]$TokenPath = ".\.agents\secrets\gmail_token.json",
    [int]$Port = 8585
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# ── 1. client_secret.json 로드 ───────────────────────────────────────────────
if (-not (Test-Path $ClientSecretPath)) {
    Write-Error @"
[ERROR] client_secret.json 파일을 찾을 수 없습니다: $ClientSecretPath

설정 방법:
  1. https://console.cloud.google.com/ 에서 프로젝트 생성
  2. Gmail API 활성화
  3. OAuth 동의 화면 설정 (외부 → 테스트 사용자에 본인 계정 추가)
  4. 사용자 인증 정보 > OAuth 클라이언트 ID 만들기 > '데스크탑 앱' 선택
  5. JSON 다운로드 → .agents/secrets/client_secret.json 으로 저장
"@
    exit 1
}

$clientSecretRaw = Get-Content $ClientSecretPath -Raw -Encoding UTF8 | ConvertFrom-Json

# installed 키 (데스크탑 앱) 또는 web 키 지원
$clientData = $null
if ($clientSecretRaw.PSObject.Properties["installed"]) {
    $clientData = $clientSecretRaw.installed
}
elseif ($clientSecretRaw.PSObject.Properties["web"]) {
    $clientData = $clientSecretRaw.web
}
else {
    Write-Error "[ERROR] client_secret.json 형식이 올바르지 않습니다. 'installed' 또는 'web' 키가 필요합니다."
    exit 1
}

$clientId = $clientData.client_id
$clientSecret = $clientData.client_secret

if (-not $clientId -or -not $clientSecret) {
    Write-Error "[ERROR] client_id 또는 client_secret이 누락되었습니다."
    exit 1
}

Write-Output "=== Gmail OAuth2 데스크탑 인증 설정 ==="
Write-Output "  Client ID: $($clientId.Substring(0, [Math]::Min(20, $clientId.Length)))..."

# ── 2. 인증 URL 구성 ─────────────────────────────────────────────────────────
$redirectUri = "http://localhost:$Port"
$scope = "https://www.googleapis.com/auth/gmail.send"
$authUrl = "https://accounts.google.com/o/oauth2/v2/auth?" +
"client_id=$([Uri]::EscapeDataString($clientId))" +
"&redirect_uri=$([Uri]::EscapeDataString($redirectUri))" +
"&response_type=code" +
"&scope=$([Uri]::EscapeDataString($scope))" +
"&access_type=offline" +
"&prompt=consent"

# ── 3. 로컬 HTTP 리스너 시작 ─────────────────────────────────────────────────
$listener = New-Object System.Net.HttpListener
$listener.Prefixes.Add("$redirectUri/")
try {
    $listener.Start()
}
catch {
    Write-Error "[ERROR] 포트 $Port 에서 HTTP 리스너를 시작할 수 없습니다. 다른 포트를 -Port 파라미터로 지정하세요.`n$_"
    exit 1
}

Write-Output ""
Write-Output "브라우저에서 Google 인증 페이지를 엽니다..."
Write-Output "  (자동으로 열리지 않으면 아래 URL을 직접 복사하여 브라우저에 붙여 넣으세요)"
Write-Output ""
Write-Output $authUrl
Write-Output ""

# 브라우저 자동 열기
Start-Process $authUrl

Write-Output "Google 계정으로 로그인하고 권한을 허용하면 자동으로 진행됩니다..."
Write-Output "(대기 중... Ctrl+C로 취소)"

# ── 4. 인증 코드 수신 ─────────────────────────────────────────────────────────
$context = $listener.GetContext()
$request = $context.Request

# 응답 페이지 전송
$responseHtml = [System.Text.Encoding]::UTF8.GetBytes(
    "<html><body><h2>인증 완료!</h2><p>이 창을 닫고 PowerShell로 돌아가세요.</p></body></html>"
)
$context.Response.ContentType = "text/html; charset=utf-8"
$context.Response.ContentLength64 = $responseHtml.Length
$context.Response.OutputStream.Write($responseHtml, 0, $responseHtml.Length)
$context.Response.OutputStream.Close()
$listener.Stop()

# 코드 추출
$queryParams = [System.Web.HttpUtility]::ParseQueryString($request.Url.Query)
$authCode = $queryParams["code"]

if (-not $authCode) {
    $errorMsg = $queryParams["error"]
    Write-Error "[ERROR] 인증 실패: $errorMsg"
    exit 1
}

Write-Output ""
Write-Output "[OK] 인증 코드를 수신했습니다."

# ── 5. 토큰 교환 ─────────────────────────────────────────────────────────────
Write-Output "액세스 토큰을 요청 중..."

$tokenBody = @{
    code          = $authCode
    client_id     = $clientId
    client_secret = $clientSecret
    redirect_uri  = $redirectUri
    grant_type    = "authorization_code"
}

try {
    $tokenResponse = Invoke-RestMethod -Uri "https://oauth2.googleapis.com/token" `
        -Method Post `
        -ContentType "application/x-www-form-urlencoded" `
        -Body $tokenBody
}
catch {
    Write-Error "[ERROR] 토큰 교환 실패: $_"
    exit 1
}

# ── 6. 토큰 저장 ─────────────────────────────────────────────────────────────
$tokenDir = Split-Path $TokenPath -Parent
if (-not (Test-Path $tokenDir)) {
    New-Item -ItemType Directory -Path $tokenDir -Force | Out-Null
}

$tokenData = @{
    access_token  = $tokenResponse.access_token
    refresh_token = $tokenResponse.refresh_token
    token_type    = $tokenResponse.token_type
    expires_in    = $tokenResponse.expires_in
    scope         = $tokenResponse.scope
    client_id     = $clientId
    client_secret = $clientSecret
    created_at    = (Get-Date -Format "o")
}

$tokenData | ConvertTo-Json -Depth 5 | Set-Content -Path $TokenPath -Encoding UTF8

Write-Output "[SUCCESS] 토큰이 저장되었습니다: $TokenPath"
Write-Output ""

# ── 7. 보안 안내 ─────────────────────────────────────────────────────────────
Write-Output "=== 보안 주의사항 ==="
Write-Output "  [!] .agents/secrets/ 디렉토리를 .gitignore에 추가하세요:"
Write-Output "      echo '.agents/secrets/' >> .gitignore"
Write-Output ""
Write-Output "  [!] gmail_token.json과 client_secret.json은 절대 Git에 커밋하지 마세요."
Write-Output ""
Write-Output "설정 완료! 이제 qa_send_email.ps1을 실행하면 Gmail API로 이메일을 전송합니다."
