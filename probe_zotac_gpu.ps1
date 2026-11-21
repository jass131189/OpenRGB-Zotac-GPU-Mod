# Run in PowerShell (Administrator recommended)
Write-Host "Checking NVIDIA RTX 5070 Ti PCI Subsystem IDs..." -ForegroundColor Cyan
Get-PnpDevice -Class Display | Where-Object { $_.InstanceId -match "VEN_10DE" } | ForEach-Object {
    $ids = Get-PnpDeviceProperty -InstanceId $_.InstanceId -KeyName "DEVPKEY_Device_HardwareIds"
    [PSCustomObject]@{
        Name        = $_.FriendlyName
        Status      = $_.Status
        HardwareIds = ($ids.Data -join " | ")
    }
} | Format-List

Write-Host "Checking for conflicting ZOTAC FireStorm / Spectra processes..." -ForegroundColor Yellow
$conflicts = Get-Process | Where-Object { $_.ProcessName -match "FireStorm|Zotac|Spectra" }
if ($conflicts) {
    $conflicts | Select-Object ProcessName, Id | Format-Table
    Write-Host "Close FireStorm before launching OpenRGB so NVAPI I2C is not locked." -ForegroundColor Red
} else {
    Write-Host "No active FireStorm process locking the I2C bus." -ForegroundColor Green
}
