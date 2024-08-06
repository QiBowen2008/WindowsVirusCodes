@echo off
echo makeC - CPU Identifier/C Builder  Version 1.05 (c) 1994,95 by B-coolWare.
if "%1" == "S" goto Small
if "%1" == "m" goto Med
if "%1" == "M" goto Med
if "%1" == "c" goto Comp
if "%1" == "C" goto Comp
if "%1" == "l" goto Large
if "%1" == "L" goto Large
if "%1" == "h" goto Huge
if "%1" == "H" goto Huge
goto Help
:Small
set model=__SMALL__
goto okModel
:Med
set model=__MEDIUM__
goto okModel
:Comp
set model=__COMPACT__
goto okModel
:Large
set model=__LARGE__
goto okModel
:Huge
set model=__HUGE__
:okModel
echo:
rem	--- setting INCLUDE and LIB environment variables. Some compilers
rem	--- require these variables to exist, so we will not override them
rem	--- if they are already defined.
if .%include%. == .. goto setInc
:checkLib
if .%lib%. == .. goto setLib
goto okPaths
:setInc
set include=Y:\BORLANDC\INCLUDE
set incdef=1
goto checkLib
:setLib
set lib=Y:\BORLANDC\LIB
set libdef=1
:okPaths
yesno Do you want to compile CPU Identifier
if errorlevel 1 goto compile
yesno Do you want to make TMi0SDGL library
if errorlevel 1 goto mklib
goto P5Info
:mklib
call mlib cputype %1
goto P5Info
:compile
echo Building CPU Identifier/C...
set makeC=ON
call mlib cputype %1
bcc -m%1 -c -I%include% cpu.c
REM tcc -m%1 -c -I%include% cpu
rem					      ^-- unREM this if you're TC user
REM cl /A%1 /c /Ox /FPi /I%include% cpu
rem					      ^-- unREM this if you're MSC user

tlink /x /L%lib% c0%1 cpu, cpuc,,c%1.lib math%1.lib emu.lib cputype%1.lib
REM tlink /x /L%lib% c0%1 cpu, cpuc,,c%1.lib math%1.lib emu.lib cputype%1.lib
rem					      ^-- unREM this if you're TC user
REM tlink /x /L%lib% cpu, cpuc,,%1libce.lib cputype%1.lib
rem					      ^-- unREM this if you're MSC user
:P5Info
echo:
yesno Do you want to compile P5Info program
if errorlevel 1 goto mkP5
goto Done
:mkP5
if "%1" == "t" goto Tiny
echo Building P5Info/C...
tasm /t/m/d%model% p5info, p5c
bcc -m%1 -c -I%include% p5info.c
REM tcc -m%1 -c -I%include% p5info
rem					      ^-- unREM this if you're TC user
REM cl /A%1 /c /Ox /FPi /I%include% p5info
rem					      ^-- unREM this if you're MSC user
tlink /x/L%lib% c0%1 p5info p5c,p5info,,c%1.lib
REM tlink /x/L%lib% c0%1 p5info p5c,p5info,,c%1.lib
rem					      ^-- unREM this if you're TC user
REM tlink /x/L%lib% p5info p5c,p5info,,%1libce.lib
rem					      ^-- unREM this if you're MSC user
:Done
if exist *.obj del *.obj >nul
echo makeC done.
set makeC=
goto Quit
:help
echo:
echo usage: makeC Model
echo:
echo   where Model can be one of these: s, c, m, l, h
:Quit
set model=
if .%incdef%.==.. goto noInc
set incdef=
set include=
:noInc
if .%libdef%.==.. goto noLib
set libdef=
set lib=
:noLib
