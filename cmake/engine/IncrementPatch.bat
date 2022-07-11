@echo off
set "major="
set "minor="
set "patch="
for /F "delims=" %%i in (%1) do if not defined major set "major=%%i"
for /F "skip=1 delims=" %%i in (%1) do if not defined minor set "minor=%%i"
for /F "skip=2 delims=" %%i in (%1) do if not defined patch set "patch=%%i"

set majorNum=%major:~14,100%
set minorNum=%minor:~14,100%
set patchNum=%patch:~14,100%

set majorNum=%majorNum: =%
set minorNum=%minorNum: =%
set patchNum=%patchNum: =%

set /A patchNum+=1

echo on version %majorNum%.%minorNum%.%patchNum%

echo VERSION_MAJOR %majorNum% > %1
echo VERSION_MINOR %minorNum% >> %1
echo VERSION_PATCH %patchNum% >> %1


 