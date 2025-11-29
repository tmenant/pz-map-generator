@echo off

set LOGFILE=%TEMP%\zomboid_build_log.txt

call helpers\build.bat > "%LOGFILE%" 2>&1

if %ERRORLEVEL% neq 0 (
    type "%LOGFILE%"
    exit /b 1
)

del "%LOGFILE%"

build\msvc-debug\Debug\pz-map-generator.exe %*