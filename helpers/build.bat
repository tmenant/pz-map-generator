@echo off

set PRESET_NAME=msvc-debug

cmake --preset %PRESET_NAME%
cmake --build build/%PRESET_NAME%