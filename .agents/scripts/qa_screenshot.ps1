param (
    [string]$Output = "QA_Result.png"
)

Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing

# Get the primary screen bounds
$bounds = [System.Windows.Forms.Screen]::PrimaryScreen.Bounds
$bitmap = New-Object System.Drawing.Bitmap $bounds.width, $bounds.height
$graphics = [System.Drawing.Graphics]::FromImage($bitmap)

# Capture the screen
$graphics.CopyFromScreen($bounds.Location, [System.Drawing.Point]::Empty, $bounds.size)

# Save the captured image
$absolutePath = [System.IO.Path]::GetFullPath($Output)
$bitmap.Save($absolutePath, [System.Drawing.Imaging.ImageFormat]::Png)

$graphics.Dispose()
$bitmap.Dispose()

Write-Output "Screenshot saved to $absolutePath"
