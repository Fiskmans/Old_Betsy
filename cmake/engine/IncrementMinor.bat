@echo off
set "major="
set "minor="
for /F "delims=" %%i in (Version.txt) do if not defined major set "major=%%i"
for /F "skip=1 delims=" %%i in (Version.txt) do if not defined minor set "minor=%%i"

set majorNum=%major:~14,100%
set minorNum=%minor:~14,100%

set majorNum=%majorNum: =%
set minorNum=%minorNum: =%

set /A minorNum+=1

echo on version %majorNum%.%minorNum%.0

echo VERSION_MAJOR %majorNum% > version.txt
echo VERSION_MINOR %minorNum% >> version.txt
echo VERSION_PATCH 0 >> version.txt
 