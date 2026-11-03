@echo off
rem Runs IdiotsTogether automation tests headless. Build first (Scripts\build.bat).
rem Usage: Scripts\test.bat [TestFilter]      default filter: IdiotsTogether
rem Requires UE_ROOT, e.g.:  set "UE_ROOT=C:\Program Files\Epic Games\UE_5.8"
setlocal

if not defined UE_ROOT (
    echo [test] UE_ROOT is not set. Point it at your Unreal Engine 5.8 install folder.
    exit /b 1
)

set "FILTER=%~1"
if "%FILTER%"=="" set "FILTER=IdiotsTogether"

set "PROJECT=%~dp0..\Game\IdiotsTogether.uproject"
set "REPORT_DIR=%~dp0..\Game\Saved\Automation\Reports"

"%UE_ROOT%\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "%PROJECT%" ^
    -ExecCmds="Automation RunTests %FILTER%" ^
    -TestExit="Automation Test Queue Empty" ^
    -ReportExportPath="%REPORT_DIR%" ^
    -unattended -nullrhi -nosplash -nosound -NoP4 -log
set "RESULT=%ERRORLEVEL%"

if "%RESULT%"=="0" (
    echo [test] PASSED. Report: %REPORT_DIR%
) else (
    echo [test] FAILED with exit code %RESULT%. Report: %REPORT_DIR%
)
exit /b %RESULT%
