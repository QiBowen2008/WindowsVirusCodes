

 EXEDAT for programmers V1.0 (with all C/C++ sources)


 You may have noticed, that almost all demogroups produce a demo in
 just one BIG .exe file.
 The big .exe file contains all code, pictures, music, etc..
 This data is probably compressed in the .exe, and decompressed at run-time.
 
 The nice thing about only one .exe file is that the demo doesn't have
 to look for his datafiles, which can be renamed or deleted.

 
 This is what EXEDAT V1.0 offers to you for FREE!


 With EXEDAT, you can easily create an archive, insert numberous
 files, compress them (or not) and link the archive to your .exe file.
 You can load and decompress the data in your program at run-time with the
 shipped C sources.
 The sources take care of all the loading and decompressing.
 Just look at ..\exeface\example.c how simple this can be done.

 What happens in the PC when you run example.exe, with test.res (example
 archive) linked to it?
 Well, let's say that just after compilation example.exe is 11000 bytes.
 With test.res connected to it, it is 12000 bytes.
 When you run example.exe, DOS loads only the 11000 bytes in memory. The
 connected data is not loaded in memory.
 At run-time, example.exe knows it's own filename (argv[0]), so it can
 load data from it's own .exe file and decompress it. This is done by the
 supplied C sources.

 You will find 4 directories in de zipfile:
 1) EXEDAT   --> contains EXEDAT archiver with C++ sources.
 2) EXEFACE  --> contains sources and example of decompression
                 of archive data at run-time.
 3) CONNECT  --> contains the connect utility with C source.
                 This utility can connect the archive to your .exe file.
 4) DATA     --> contains example data for example.c

 Normally, you don't have to use the source in the EXEDAT
 directory, just use EXEDAT.EXE to create the archive.
 The source of EXEDAT were shipped, so that you can solve any
 possible bugs in the code(I hope not!).
 
 The sources, that you use a lot are the ones in EXEFACE.
 Just look at example.c, and see how simple it works.

 EXEDAT.EXE knows three archive-methods:
 1) compr_none  0  ; no compression
 2) compr_lzss  1  ; lzss compression, for fast decoding
 3) compr_lzari 2  ; arithmetic compression, like ZIP, ARJ

 EXEDAT.EXE Usage by example:
 exedat n test.res              :create a new archive called test.res
 exedat a test.res file1.txt 0  :add file1.txt in test.res with no compression
 exedat a test.res file2.txt 1  :              ,,          with lzss compr.
 exedat a test.res file3.txt 2  :              ,,          with arithm. compr.
 exedat l test.res              :list contents of test.res
 exedat d test.res file1.txt    :delete file1.txt from archive test.res
 exedat x test.res file1.txt    :extract(no delete) file1.txt from test.res
 
 note: EXEDAT.EXE can only be run on 386+ machines.
       Recompile for use with other processors.

 The connect utility:
 In the CONNECT directory you find connect.c and connect.exe.
 With this program, you can connect the archive to your .exe file.
 Usage:   connect.exe exefile.exe archive.res

 Some hints:
 While developing your program, it is not smart to link your archive to
 your .exe every time, because it's time-consuming.
 Just keep the archive in the same directory.
 In de example.c code, you see the line:   exedat_archive(argv[0])
 This means that the archive filename is found at the end of it's own .exe
 Change this code line in: exedat_archive("test.res"); //or your archive name
 When the program or demo is completed, than you change the line back, and
 then link the archive to your program.

 Compressing your exe-file:
 When you want to compress your program, with PKLITE or something, then you
 must do this BEFORE you link the archive to it. With the archive also would
 not make sense, because in the archive is compressed data.
 
 Credits:
 Credits for lzss and lzari go to Haruhiko Okumura. These sources are free
 of copurights.
 All the other code is done by me, Ren‚ Olsthoorn.

 Copyrights:
 EXEDAT V1.0 is totally FREE!
 Do with it what you want. Distribute it. Enhance it. You have my blessing.

 Compilation of sourcecode:
 In every directory with sourcecode, you will find several batchfiles.
 With those, you can compile the code. Choose one.
 Use makewat.bat if you have WATCOM C/C++ (tested with 10.0).
 Use makegcc.bat if you have DJGPP2 (which is FREE!).
 Use makebor.bat if you have Borland C/C++.
 Notice that the 16-bit borland users have got the 64kram restriction.

 Hint: If you only got Borland: Get DJGPP2! It's free, and 32-bit!
 If you got enough money, then buy WATCOM C/C++. It is perfect.
 (WATCOM 10.5 is only $199 nowadays!)

 WATCOM C/C++ users:
 Notice that there's a standalone protected mode stub for WATCOM.
 It's called PMODEW (10kram). This stub is linked with your exe-file.
 So you don't need the bulky DOS4GW anymore.
 Download PMODEW from ftp:  x2ftp.oulu.fi in pub/msdos/programming/watcom
 EXEDAT is compiled with WATCOM/PMODEW.

 Any questions,
 e-mail me before June 1997 at reneo@htsa.hva.nl

 Greetings:
    Rene Olsthoorn.
