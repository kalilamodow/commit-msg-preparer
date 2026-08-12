cl /Zi /Od src\main.c /GS- /link kernel32.lib comctl32.lib user32.lib gdi32.lib /DEBUG /ENTRY:CustomEntry /SUBSYSTEM:CONSOLE /NODEFAULTLIB /out:target.exe
