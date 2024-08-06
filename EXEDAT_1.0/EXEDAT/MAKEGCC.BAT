rem Batchfile for compiling with DJGPP2, the FREE 32-bit compiler!

gxx -fpack-struct -o exedat.exe exedat.cpp lzss.c lzari.c globals.c -lgpp -lm
