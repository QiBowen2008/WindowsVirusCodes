rem Batchfile for compiling with WATCOM C/C++ (tested with 10.0)
rem    if you don't have the PMODEW stub (standalone PMODE/ NO DOS4GW!)
rem    for WATCOM then use /l=dos4g, but you then must have (bulky) DOS4GW
rem    contact ftp: x2ftp.oulu.fi for shareware PMODEW.

wcl386 /l=pmodew /fe=exedat.exe exedat.cpp lzss.c lzari.c globals.c
