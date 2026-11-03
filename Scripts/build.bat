@echo off
rem Builds the IdiotsTogetherEditor target (Win64 Development).
rem Requires UE_ROOT, e.g.:  set "UE_ROOT=C:\Program Files\Epic Games\UE_5.8"
setlocal

if not defined UE_ROOT (
    echo [build] UE_ROOT is not set. Point it at your Unreal Engine 5.8 install folder.
    exit /b 1
)

set "PROJECT=%~dp0..\Game\IdiotsTogether.uproject"

call "%UE_ROOT%\Engine\Build\BatchFiles\Build.bat" IdiotsTogetherEditor Win64 Development -Project="%PROJECT%" -WaitMutex
set "RESULT=%ERRORLEVEL%"

if not "%RESULT%"=="0" echo [build] FAILED with exit code %RESULT%
exit /b %RESULT%
