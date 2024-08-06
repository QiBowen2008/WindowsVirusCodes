/* ------------------------------------------------------------------------- *
 * CPU.C  Sample program demonstrating usage of TMi0SDGL(tm) routines        *
 * 									     *
 * Copyright(c) 1994,95 by B-coolWare.  Written by Bobby Z.		     *
 * ------------------------------------------------------------------------- *
 * files needed to build project:
   CPU.C
   CPUTYPE.C
   CPU_HL.ASM
   CPUSPEED.ASM
   SMM.ASM
*/

#include <stdio.h>
#include "cputype.h"

char* SMM[2] = {"","  [SL Enhanced]"};

void main()
{
 puts("CPU Type Identifier/C  Version 1.17  Copyright(c) 1992-95 by B-coolWare.\n");

 /* due to parameters passing convention CPU_Speed() routine will be called
    first, when _CPU is not yet defined. So we should call cpuType_Str
    before invoking CPU_Speed() */
 printf("  Processor: %s, ",cpuType_Str());
 printf("%dMHz%s\n",intCPU_Speed(),SMM[isSMMAble()]);
 if (IsCyrix())
  printf("  Cyrix CPU Step: %d, Revision: %d\n",CxStep(),CxRevision());
 printf("Coprocessor: %s\n",fpuType_Str());
}
