@echo off

cl src\main.c /link /out:target.exe && .\target.exe

echo.
echo.
echo error code: %ERRORLEVEL%
