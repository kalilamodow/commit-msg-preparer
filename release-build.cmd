@echo off

cl /O1 src\main.c /GS- /link kernel32.lib comctl32.lib user32.lib gdi32.lib /ENTRY:CustomEntry /SUBSYSTEM:CONSOLE /NODEFAULTLIB /out:target.exe

echo.
echo.
echo error code: %ERRORLEVEL%
