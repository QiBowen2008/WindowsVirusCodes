@echo off
echo makeA - CPU Identifier/Asm Builder  Version 1.01 (c) 1994,95 by B-coolWare.
echo:
yesno Do you want to compile CPU Identifier
if errorlevel 1 goto compile
goto Done
:compile
echo Building CPU Identifier/Asm...
tasm /t/m/d__TINY__/r cpu
tlink /t/x cpu
if exist *.obj del cpu.obj >nul
:Done
echo makeA done.
