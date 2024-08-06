/* ------------------------------------------------------------------------- *
 * CPUTYPE.H  TMi0SDGL(tm) interface module		      Version 1.17   *
 *									     *
 * Copyright(c) 1994,95 by B-coolWare.  Written by Bobby Z.		     *
 * ------------------------------------------------------------------------- */

#ifndef __CPUTYPE_H
#define __CPUTYPE_H

/*
#define __NEED_EXACT_MHZ__
*/

typedef unsigned char byte;
typedef unsigned int  word;

#ifndef __Lib__

#ifdef __cplusplus
extern "C" {
#endif
extern char * pascal cpuType_Str(void);
extern char * pascal fpuType_Str(void);
extern byte pascal CxStep(void);
extern byte pascal CxRevision(void);

#ifdef __NEED_EXACT_MHZ__
extern float pascal CPU_Speed(void);
#endif

extern int pascal intCPU_Speed(void);
extern int pascal isSMMAble(void);
#ifdef __cplusplus
}
#endif

#endif /* __Lib__ */


/* processor and coprocessor type defines follow */

#define i8088		0x0000
#define i8086		0x0001
#define V20		0x0002
#define V30		0x0003
#define i80188		0x0004
#define i80186		0x0005
#define i80286		0x0006
#define i80386sxr	0x0007
#define i80386sxv	0x0107
#define i80386dxr	0x0008
#define i80386dxv	0x0108
#define i386slr		0x0009
#define i386slv		0x0109
#define i486sxr		0x000A
#define i486sxv		0x010A
#define i486dxr		0x000B
#define i486dxv		0x010B
#define cx486slcr	0x000C
#define cx486slcv	0x010C
#define cx486r		0x000D
#define cx486v		0x010D
#define iP5r		0x000E
#define iP5v		0x010E
#define cxM1r		0x000F
#define cxM1v		0x010F
#define iP24Tr		0x0010
#define iP24Tv		0x0110
#define ibm386slcr	0x0011
#define ibm386slcv	0x0111
#define ibm486slcr	0x0012
#define ibm486slcv	0x0112
#define ibm486slc2r	0x0013
#define ibm486slc2v	0x0113
#define umcU5sxr	0x0014
#define umcU5sxv	0x0114
#define umcU5dxr	0x0015
#define umcU5dxv	0x0115
#define am386sxr	0x0016
#define am386sxp	0x0116
#define am386dxr	0x0017
#define am386dxp	0x0117
#define nx586r		0x0018
#define nx586p		0x0118
#define ibm486bl3r	0x0019
#define ibm486bl3v	0x0119
#define Am486r		0x001A
#define Am486v		0x011A
#define iP54r		0x001B
#define iP54Tv		0x011B
#define P6r		0x001C
#define P6p		0x011C


#ifdef __cplusplus
extern "C" {
#endif
extern word pascal CPU_Type(void);
extern byte pascal checkEmu(void);
extern byte pascal IsCyrix(void);
#ifdef __cplusplus
}
#endif

#ifndef __Lib__
extern byte pascal __CPU;
#else
byte pascal __CPU = 0;
#endif

#endif /* __CPUTYPE_H */
