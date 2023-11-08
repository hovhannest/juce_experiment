param (
    [string]$exePath
)


# Check if the $exePath parameter is provided
if (-not $exePath) {
    Write-Host "Please provide the path to the executable as an argument."
    Exit
}

# Get a list of all processes with the specified .exe path
$processes = Get-Process | Where-Object { $_.MainModule.FileName -eq $exePath }

# Terminate each process
$processes | ForEach-Object { Stop-Process -Id $_.Id -Force }
