@echo off
set "major="
for /F "delims=" %%i in (Version.txt) do if not defined major set "major=%%i"

set majorNum=%major:~14,100%

set majorNum=%majorNum: =%

set /A majorNum+=1

echo on version %majorNum%.0.0

echo VERSION_MAJOR %majorNum% > version.txt
echo VERSION_MINOR 0 >> version.txt
echo VERSION_PATCH 0 >> version.txt
 