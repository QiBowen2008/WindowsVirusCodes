@echo off
echo mLib - TMi0SDGL(tm) Library Builder  Version 1.05 (c) 1994,95 by B-coolWare.
echo:
if .%1. == .. goto Help
if .%model%.==.. goto setModel
goto okModel
:setModel
if "%2" == "s" goto Small
if "%2" == "S" goto Small
if "%2" == "m" goto Med
if "%2" == "M" goto Med
if "%2" == "c" goto Comp
if "%2" == "C" goto Comp
if "%2" == "l" goto Large
if "%2" == "L" goto Large
if "%2" == "h" goto Huge
if "%2" == "H" goto Huge
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
rem	--- setting INCLUDE environment variable. Some compilers
rem	--- require this variable to exist, so we will not override it
rem	--- if it is already defined.
if .%include%. == .. goto setInc
goto okPaths
:setInc
set include=Y:\BORLANDC\INCLUDE
set incdef2=1
:okPaths
echo Building C/C++ TMi0SDGL(tm) Library...
tasm /t/m/d%model% cpuspeed, speed_c
tasm /t/m/d%model% cpu_hl, cpu_c
tasm /t/m/d%model% p5info
tasm /t/m/d%model% smm
tasm /t/m/d%model% cxmodel
bcc -m%2 -c -I%include% cputype.c
REM tcc -m%2 -c -I%include% cputype
rem				    	      ^-- unREM this if you're TC user
REM cl /A%2 /c /Ox /FPi /I%include% cputype  
rem                                           ^-- unRem this if you're MSC user
if exist %1%2.lib del %1%2.lib
tlib %1%2 /C /0 +speed_c.obj +cpu_c.obj +cputype.obj +p5info.obj +smm.obj +cxmodel.obj
echo mLib done.
goto Quit
:Help
echo   usage: mlib libname s³c³m³l³h
:Quit
if %makeC% == ON goto Done
set model=
:Done
if .%incdef2%.==.. goto noInc
set incdef2=
set include=
:noInc
