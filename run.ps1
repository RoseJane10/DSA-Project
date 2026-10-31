
# ============================================
# BEAUTRIX SYSTEM LAUNCHER
# ============================================

# 1. Go to the project folder
Set-Location $PSScriptRoot

# 2. Add MSYS2 UCRT64 to PATH
$env:Path = "C:\msys64\msys2\ucrt64\bin;$env:Path"

# 3. Load environment variables from .env
if (Test-Path ".env") {

    Get-Content ".env" | ForEach-Object {

        if ($_ -match '^\s*([^#][^=]*)=(.*)$') {

            [Environment]::SetEnvironmentVariable(
                $matches[1].Trim(),
                $matches[2].Trim(),
                "Process"
            )
        }
    }

} else {

    Write-Host "ERROR: .env file not found!" -ForegroundColor Red
    exit 1
}

# 4. Check database configuration
if (-not $env:BEAUTRIX_DB) {

    Write-Host "ERROR: Database configuration missing!" -ForegroundColor Red
    exit 1
}

# 5. Check if application exists
if (-not (Test-Path ".\build\beautrix.exe")) {

    Write-Host "ERROR: BEAUTRIX has not been built yet!" -ForegroundColor Red
    Write-Host "Run: cmake --build build"
    exit 1
}

# 6. Launch application
Write-Host ""
Write-Host "============================================" -ForegroundColor Cyan
Write-Host "           BEAUTRIX SYSTEM" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
Write-Host ""

& ".\build\beautrix.exe"
