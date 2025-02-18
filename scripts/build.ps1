#!/usr/bin/env pwsh

$SCRIPT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Path
$ROOT_DIR = Split-Path -Parent $SCRIPT_DIR
$BUILD_DIR = Join-Path $ROOT_DIR "build"
$VCPKG_ROOT = Join-Path $ROOT_DIR "vcpkg"

New-Item -ItemType Directory -Force -Path $BUILD_DIR | Out-Null
New-Item -ItemType Directory -Force -Path $VCPKG_ROOT | Out-Null

if (-not (Test-Path (Join-Path $VCPKG_ROOT "vcpkg.exe"))) {
    Write-Host "Installing vcpkg..."
    git clone https://github.com/Microsoft/vcpkg.git $VCPKG_ROOT
    Push-Location $VCPKG_ROOT
    .\bootstrap-vcpkg.bat
    Pop-Location
}

$env:VCPKG_ROOT = $VCPKG_ROOT

Push-Location $BUILD_DIR
cmake -B . -S $ROOT_DIR `
    -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
    -DVCPKG_TARGET_TRIPLET=x64-windows `
    -DCMAKE_BUILD_TYPE=Release

cmake --build . --config Release
Pop-Location

Write-Host "Build finished."
Write-Host "Build directory: $BUILD_DIR"
Write-Host "Vcpkg directory: $VCPKG_ROOT" 