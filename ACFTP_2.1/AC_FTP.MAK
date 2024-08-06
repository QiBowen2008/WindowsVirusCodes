# Microsoft Visual C++ generated build script - Do not modify

PROJ = AC_FTP
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = .
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = AC_FTP.C    
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W4 /Zi /AL /Od /D "_DEBUG" /FR /GA /Fd"AC_FTP.PDB"
CFLAGS_R_WEXE = /nologo /W4 /AL /O1 /D "NDEBUG" /GA 
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /MAP /LINE  
LIBS_D_WEXE = oldnames libw llibcew commdlg.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WEXE = oldnames libw llibcew commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = AC_FTP.DEF
OBJS_EXT = 
LIBS_EXT = ..\WINSOCK.LIB ..\WINSOCKX.LIB 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = AC_FTP.SBR \
		FTP_CTRL.SBR \
		FTP_DATA.SBR


WINSOCK_DEP = 

WINSOCKX_DEP = 

AC_FTP_DEP = ..\wsa_xtra.h \
	..\winsockx.h \
	ac_ftp.h


AC_FTP_RCDEP = ac_ftp.h \
	..\winsockx.h \
	ac_ftp.ico


FTP_CTRL_DEP = ..\wsa_xtra.h \
	..\winsockx.h \
	ac_ftp.h


FTP_DATA_DEP = ..\wsa_xtra.h \
	..\winsockx.h \
	ac_ftp.h


all:	$(PROJ).EXE

AC_FTP.OBJ:	AC_FTP.C $(AC_FTP_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c AC_FTP.C

AC_FTP.RES:	AC_FTP.RC $(AC_FTP_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r AC_FTP.RC

FTP_CTRL.OBJ:	FTP_CTRL.C $(FTP_CTRL_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c FTP_CTRL.C

FTP_DATA.OBJ:	FTP_DATA.C $(FTP_DATA_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c FTP_DATA.C


$(PROJ).EXE::	AC_FTP.RES

$(PROJ).EXE::	AC_FTP.OBJ FTP_CTRL.OBJ FTP_DATA.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
AC_FTP.OBJ +
FTP_CTRL.OBJ +
FTP_DATA.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
..\WINSOCK.LIB+
..\WINSOCKX.LIB+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) AC_FTP.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	AC_FTP.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) AC_FTP.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)

clean: 
       del *.exe
       del *.obj
       del *.res
       del *.pdb
       del *.sbr

$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
