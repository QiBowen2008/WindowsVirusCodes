/* ------------------------------------------------------------------------- *
 * P5INFO.C  Intel P5 and like CPU feature list			Version 1.04 *
 *                                                                           *
 * Copyright(c) 1994,95 by B-coolWare.  Written by Bobby Z.                  *
 * ------------------------------------------------------------------------- *
 * Files needed to build:

   HEADER.ASH	- assembler header file for P5INFO.ASM
   P5INFO.ASM	- low-level routines
   P5INFO.C	- this file

   How to build:

   Assemble P5INFO.ASM with TASM 2.0 or compatible assembler (check the memory
   model in HEADER.ASH before assembling!), then compile P5INFO.C with any
   C/C++ compiler and link them both with startup code (c0x.obj for 
   Turbo/Borland C[++]).
*/

#include <stdio.h>
#include <conio.h>

typedef unsigned int  word;

#define Family 0x0F00
#define Model  0x00F0
#define Step   0x000F

#define FPUonChip 	0x0001
#define EnhancedV86 	0x0002
#define IOBreakpoints 	0x0004
#define PageSizeExt 	0x0008
#define TimeStampCnt	0x0010
#define ModelSpecific	0x0020
#define MachineCheckExc	0x0080
#define CMPXCHG8B	0x0100
#define APIConChip	0x0200

#ifdef	__cplusplus
extern "C" {
#endif
extern word pascal CheckP5(void);
/* returns zero if CPUID not supported */

extern word pascal CheckP5_2(void);
/* returns -1 if CPUID doesn't work */

extern void pascal GetP5Vendor(char far *);
/* returns Pascal-style string[12] filled with vendor ID string */

extern word pascal GetP5Features(void);
/* returns bit-encoded CPU features word */

#ifdef __cplusplus
}
#endif

char useSecondVersion = 0;

char * getVendor(void)
{
 static char s[14];
 GetP5Vendor(s);
 if(!s[0])
  return s;
 else
  {
   s[13]=0;
   return s+1;
  }
}

word getFamily(void)
{
 if( useSecondVersion )
  return ((CheckP5_2() & Family) >> 8);
 else
  return ((CheckP5() & Family) >> 8);
}

word getModel(void)
{
 if( useSecondVersion )
  return ((CheckP5_2() & Model) >> 4);
 else
  return ((CheckP5() & Model) >> 4);
}

word getStep(void)
{
 if( useSecondVersion )
  return (CheckP5_2() & Step);
 else
  return (CheckP5() & Step);
}

void printBullet(word doPrint)
{
 if(doPrint)
  putch(0xFE);
 else
  putch(' ');
}

void printFeatures(void)
{
 word Features = GetP5Features();

 printBullet(Features & FPUonChip);
 puts(" FPU on chip");

 printBullet(Features & EnhancedV86);
 puts(" Enhanced Virtual-8086 mode");

 printBullet(Features & IOBreakpoints);
 puts(" I/O Breakpoints");

 printBullet(Features & PageSizeExt);
 puts(" Page Size Extensions");

 printBullet(Features & TimeStampCnt);
 puts(" Time Stamp Counter");

 printBullet(Features & ModelSpecific);
 puts(" Pentium processor-style model specific registers");

 printBullet(Features & MachineCheckExc);
 puts(" Machine Check Exception");

 printBullet(Features & CMPXCHG8B);
 puts(" CMPXCHG8B Instruction");

 printBullet(Features & APIConChip);
 puts(" APIC on chip");
}

void main(void)
{
 char c;
 puts("P5Info/C  Version 1.04  Copyright(c) 1994,95 by B-coolWare.\n");

 if(!CheckP5())
  {
   printf("The CPU doesn't seem to handle CPUID instruction. Give it a try anyway? [Y/N] ");
   do
   {
     c = getch() & 0xDF;
   }
   while( (c != 'Y') && (c != 'N'));
   printf("%c\n\n",c);
   if (c == 'N')
    return;
   else
    if (CheckP5_2() == 0xFFFF)
     {
      puts("Oops... No way, sorry...");
      return;
     }
    else
     useSecondVersion = 1;
  }
 printf("Make %s\nFamily %d Model %d Step %d\n\nProcessor Features:\n",
        getVendor(),getFamily(),getModel(),getStep());
 printFeatures();
}
