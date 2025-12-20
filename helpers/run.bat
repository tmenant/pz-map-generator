@echo off

call "%~dp0\build.bat" >nul 2>&1

if %ERRORLEVEL% neq 0 (
    echo build failed.
    exit /b 1
)

build\mingw-debug\pz-map-generator.exe %*