@echo off

call "%~dp0\build.bat"

if %ERRORLEVEL% neq 0 (
    type "%LOGFILE%"
    exit /b 1
)

cls

build\mingw-debug\tests.exe %*

