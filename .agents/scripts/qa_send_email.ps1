param (
    [string]$To = "manager@example.com",
    [string]$From = "qa_agent@example.com",
    [string]$Subject = "QA Report",
    [string]$BodyFile = "",
    [string]$Attachment = "",
    [string]$SmtpServer = "smtp.example.com" # Change to real SMTP if testing
)

$bodyText = "QA Review Completed."
if ($BodyFile -and (Test-Path $BodyFile)) {
    $bodyText = Get-Content $BodyFile -Raw
}

Write-Output "Preparing to send email to $To via $SmtpServer..."
Write-Output "Subject: $Subject"
Write-Output "Body: $bodyText"

# Note: In a real environment, you need proper SMTP credentials.
# The Send-MailMessage can be used like below:
# 
# if ($Attachment -and (Test-Path $Attachment)) {
#     Send-MailMessage -To $To -From $From -Subject $Subject -Body $bodyText -SmtpServer $SmtpServer -Attachments $Attachment
# } else {
#     Send-MailMessage -To $To -From $From -Subject $Subject -Body $bodyText -SmtpServer $SmtpServer
# }

Write-Output "Email simulated successfully! (Setup your SMTP credentials in QA script to actually send)"
if ($Attachment) {
    Write-Output "Attached File: $Attachment"
}
