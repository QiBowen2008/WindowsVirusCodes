/* -------------------------------------------------------------------------- *
 * CPUTYPE.C  TMi0SDGL(tm) Implementation module	       Version 1.17   *
 *									      *
 * Copyright(c) 1994,95 by B-coolWare.  Written by Bobby Z. and VAP.	      *
 * -------------------------------------------------------------------------- *
 * files needed to build:
   
   CPUTYPE.C		- compile with memory model XXX
   CPU_HL.ASM		- set memory model to XXX
   CPUSPEED.ASM		- set memory model to XXX
   P5INFO.ASM		- set memory model to XXX
   SMM.ASM		- set memory model to XXX
*/

/* One define affects the way code is compiled:

   __NEED_EXACT_MHZ__

   If this is defined, then the CPU_Speed() routine will be compiled, causing
   linking of floating point library. If rough approximation to integer of
   CPU clock speed is enough for you, DO NOT uncomment this define in
   CPUTYPE.H file.
*/

#include <stdlib.h>
#include <string.h>

#define __Lib__
#include "cputype.h"

byte pascal FPUType = 0xFF;	/* variable to keep FPU test result code */
long pascal CPUFix  = 0L;	/* variable used internally in CPU speed testing */
word pascal Shift   = 2;		/* --"-- */
/*
long pascal QEMMEntry = 0L;
char pascal QEMMId[] = "QEMM386$\x0";
*/
#ifdef __cplusplus
extern "C" {
#endif
extern int pascal Speed( byte );	     /* returns raw CPU speed factor */
extern word pascal GetP5Features(void);	     /* returns P5 feature word */
extern void pascal GetP5Vendor(char far *);  /* returns CPU vendor id string */
extern word pascal CheckP5(void);	     /* checks if cpuid works */
extern word pascal getCyrixModel(void);	     /* returns Cyrix CPU model */
#ifdef __cplusplus
}
#endif

void pascal checkUMC(void)
{
 char s[14];
 if(__CPU >= i486sxr)
  if((CheckP5()&0x0F00)==0x400) /* Family == 4, don't care of other fields */
   {
    GetP5Vendor(s);
    s[13]=0;
    if(strstr(s+1,"UMC")!=0L)
     switch(CheckP5() & 0x00F0)
      {
       case 0x0020: __CPU = 0x14;
 		    break;
       case 0x0010: __CPU = 0x15;
      }
   }
}

#ifdef __NEED_EXACT_MHZ__

float pascal CPU_Speed(void)
{
 word sp;
 if(!__CPU)
  return 0;
 sp = Speed(__CPU);
 return (((long)Shift*CPUFix)/sp+5)/10;
}

#endif

int pascal intCPU_Speed(void)
{
 long sp;
 if(!__CPU)
  return 0;
 sp = (long) Speed(__CPU);
 return (int) ((((long)Shift*CPUFix)/sp+5l)/10l);
}

static char *cpuNames[] = {"Intel 8088", "Intel 8086", "NEC V20", "NEC V30",
		           "Intel 80188", "Intel 80186", "Intel 80286", "Intel 80386SX",
		           "Intel 80386DX", "IBM 386SL", "Intel i486SX", "Intel i486DX or i487SX",
		           "Cyrix Cx486SLC", "Cyrix Cx486", "Intel Pentium", "Cyrix M1 (586)",
		           "iP24T (Pentium OverDrive)","IBM 386SLC","IBM 486SLC", "IBM 486SLC2",
                           "UMC U5-S","UMC U5-D","AMD Am386SX","AMD Am386DX", "NexGen Nx586",
			   "IBM 486BL3 (Blue Lightning)", "Intel iP54", "AMD Am486DX",
                           "Intel P6"
		   };

static char *cpuModels[] = {"DX","DX","SX","DX2/OverDrive","SL","SX2","","DX2WB (P24D)","DX4","","","","","","","" };

byte pascal CxStep(void)
{
 return((getCyrixModel() & 0xF000) >> 12);
}

byte pascal CxRevision(void)
{
 return((getCyrixModel() & 0x0F00) >> 8);
}

char * pascal CxModel(void)
{
 switch(getCyrixModel() & 0xFF) {
 case 0 : return("Cyrix Cx486SLC");
 case 1 : return("Cyrix Cx486DLC");
 case 2 : return("Cyrix Cx486SL2");
 case 3 : return("Cyrix Cx486DL2");
 case 4 : return("Cyrix Cx486SR");
 case 5 : return("Cyrix Cx486DR");
 case 6 : return("Cyrix Cx486SR2");
 case 7 : return("Cyrix Cx486DR2");
 case 0x10: return("Cyrix Cx486S");
 case 0x11: return("Cyrix Cx486S2");
 case 0x12: return("Cyrix Cx486SE");
 case 0x13: return("Cyrix Cx486S2E");
 case 0x1A: 
      FPUType = (FPUType & 1) + 0x10;
      return("Cyrix Cx486DX");
 case 0x1B:
      FPUType = (FPUType & 1) + 0x10;
      return("Cyrix Cx486DX2");
 case 0x30:
      FPUType = (FPUType & 1) + 0x10;
      return("Cyrix M1 (586)");
 case 0xFE: return "Texas Instruments Ti486SXL";
default:
      return("Cyrix/Texas Instruments 486");
 }
}

char * pascal AMD486Model(void)
{
 switch(CheckP5() & 0x00F0)
  {
   case 0x0030 : return("2");
   case 0x0070 : return("2+");
   case 0x0080 : return("4");
   case 0x0090 : return("4+");
  default:
   return("");
  }
}

char * pascal cpuType_Str(void)
{
  word c = CPU_Type();
  __CPU = c;
  checkUMC();
  if (__CPU == 0x0A && (c>>12))
   return strcat("Intel i486",cpuModels[c >> 12]);
  switch(c) {
   case i80386sxr:
   case i80386sxv: if (intCPU_Speed() > 35)
 		    return cpuNames[0x16];
		   else
	 	    return cpuNames[__CPU];
   case i80386dxr:
   case i80386dxv: if (intCPU_Speed() > 35)
		    return cpuNames[0x17];
		   else
		    return cpuNames[__CPU];
   case cx486slcr:
   case cx486slcv:
   case cx486r:
   case cx486v:
 		   return(CxModel());
   case Am486r:
   case Am486v:
		   return(strcat(cpuNames[__CPU],AMD486Model()));
   default:
	           return cpuNames[__CPU];
  }
}

static char *fpuNames[] = {"Unknown", "Unknown", "None", "Weitek", "Intel 8087",
		           "Intel 8087 and Weitek", "Intel i487sx", "Intel i487sx and Weitek",
		           "Intel 80287", "Intel 80287 and Weitek", "Cyrix 82x87",
		           "Cyrix 82x87 and Weitek", "Intel 80387", "Intel 80387 and Weitek",
		           "Cyrix 83x87", "Cyrix 83x87 + Weitek", "Internal",
		           "Internal and Weitek", "Cyrix 84x87", "Cyrix 84x87 and Weitek",
		           "Intel 80287XL", "Intel 80287XL and Weitek",
		           "IIT 2C87", "IIT 2C87 and Weitek", "IIT 3C87", "IIT 3C87 and Weitek",
			   "ULSI 83x87", "ULSI 83x87 and Weitek", "Cyrix EMC87",
			   "Cyrix EMC87 and Weitek", "C&T 38700", "C&T 38700 and Weitek",
			   "NexGen Nx587", "NexGen Nx587 and Weitek", "IIT 4C87", 
                           "IIT 4C87 and Weitek"
		           };

char * pascal fpuType_Str(void)
{
  int c;
  if (FPUType == 0xFF)
  {
   c = CPU_Type();
   switch(c) {
    case cx486slcr:
    case cx486slcv:
    case cx486r:
    case cx486v:
               CxModel();
   }
  }
  if (FPUType > 0x23) 
   return ("Unknown");
  else
   if ((c >= i80286) && checkEmu())
    return strcat(fpuNames[FPUType],", Emulated");
   else
    return fpuNames[FPUType];
}
