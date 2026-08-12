@echo off

cl /Gy /Gw /O1 src\main.c /GS- /link kernel32.lib user32.lib gdi32.lib /ENTRY:CustomEntry /SUBSYSTEM:CONSOLE /NODEFAULTLIB /OPT:REF /OPT:ICF /INCREMENTAL:NO /DEBUG:NONE /ALIGN:16 /NOCOFFGRPINFO /MANIFEST:EMBED /out:target.exe

echo.
echo.
echo error code: %ERRORLEVEL%
