# Microsoft Visual C++ generated build script - Do not modify

PROJ = CHEKMAIL
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\S\CHEKMAIL\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = CHEKMAIL.C  
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AL /Od /D "_DEBUG" /FR /GA /Fd"CHEKMAIL.PDB"
CFLAGS_R_WEXE = /nologo /W3 /AL /O1 /D "NDEBUG" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO 
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = oldnames libw llibcew winsock 
LIBS_R_WEXE = oldnames libw llibcew winsock commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = CHEKMAIL.DEF
OBJS_EXT = 
LIBS_EXT = 
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
SBRS = CHEKMAIL.SBR


CHEKMAIL_DEP = c:\msvc\include\winsock.h


all:	$(PROJ).EXE $(PROJ).BSC

CHEKMAIL.OBJ:	CHEKMAIL.C $(CHEKMAIL_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c CHEKMAIL.C

CHEKMAIL.RES:	CHEKMAIL.RC $(CHEKMAIL_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r CHEKMAIL.RC


$(PROJ).EXE::	CHEKMAIL.RES

$(PROJ).EXE::	CHEKMAIL.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
CHEKMAIL.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
c:\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) CHEKMAIL.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	CHEKMAIL.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) CHEKMAIL.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
