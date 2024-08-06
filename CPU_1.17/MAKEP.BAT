@echo off
echo makeP - CPU Identifier/Pas Builder  Version 1.05 (c) 1994,95 by B-coolWare.
yesno Do you want to compile the CPU Identifier
if errorlevel 1 goto compile
goto P5Info
:compile
echo Building CPU Identifier/Pas...
tasm /t/m/d__LARGE__ cpu_hl, cpu_tp
tasm /t/m/d__LARGE__ cpuspeed, speed_tp
tasm /t/m/d__LARGE__ p5info
tasm /t/m/d__LARGE__ smm
tasm /t/m/d__LARGE__ cxmodel
tpc /m cpu
REM ------------------------------------------------- BP 7
REM tasm /t/m/d__LARGE__/d__DPMI__ cpu_hl,cpu_tp.obp
REM tasm /t/m/d__LARGE__/d__DPMI__ cpuspeed,speed_tp.obp
REM echo Building Real Mode executable...
REM bpc -cd -m cpu
REM ren cpu.exe cpu_rm.exe >nul
REM echo Building Protected Mode executable...
REM bpc -cp -m cpu
REM ren cpu.exe cpu_pm.exe >nul
REM ------------------------------------------------- BP 7
rem		^-- unREM this if you're BP 7 user
echo:
:P5Info
yesno Do you want to compile P5Info program
if errorlevel 1 goto mkP5
goto Done
:mkP5
echo Building P5Info/Pas...
if not exist p5info.obj tasm /t/m/d__LARGE__ p5info
tpc p5info
REM bpc -cd p5info
rem		^-- unREM this if you're BP 7 user
:Done
if exist *.ob? del *.ob? >nul
echo makeP done.
