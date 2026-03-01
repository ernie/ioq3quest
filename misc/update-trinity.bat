@echo off
REM Downloads the latest Trinity mod pk3s from GitHub into the
REM Android assets directory for local builds.

set "REPO=ernie/trinity"
set "BASE_URL=https://github.com/%REPO%/releases/latest/download"

set "SCRIPT_DIR=%~dp0"
set "ASSETS=%SCRIPT_DIR%..\android\app\src\main\assets"

echo Downloading latest Trinity pk3s...
curl -fL -o "%ASSETS%\pak8t.pk3" "%BASE_URL%/pak8t.pk3" && echo   pak8t.pk3 OK || echo   pak8t.pk3 FAILED
curl -fL -o "%ASSETS%\pak3t.pk3" "%BASE_URL%/pak3t.pk3" && echo   pak3t.pk3 OK || echo   pak3t.pk3 FAILED

echo Done.
pause
