{ --------------------------------------------------------------------------- }
{ P5INFO.PAS  Intel P5 and like CPU feature lister		 Version 1.04 }
{								              }
{ Copyright(c) 1994,95 by B-coolWare.  Written by Bobby Z.	              }
{ --------------------------------------------------------------------------- }
{
  Files needed to build project:

  HEADER.ASH	- assembler header file
  P5INFO.ASM	- low-level routines
  P5INFO.PAS 	- this file

  How to build:

  Assemble P5INFO.ASM with TASM 2.0 or any other compatible assembler (check
  memory model in HEADER.ASH before assembling!), then compile P5INFO.PAS with
  Turbo/Borland or Stony Brook Pascal compiler.

}
{$S-,I-,V-,X+}
program P5Info;

uses Crt;

const
      FPUonChip         = $0001;
      EnhancedV86       = $0002;
      IOBreakpoints     = $0004;
      PageSizeExtensions= $0008;
      TimeStampCounter  = $0010;
      ModelSpecificRegs = $0020;
      MachineCheckExcept= $0080;
      CMPXCHG8B         = $0100;
      APIConChip	= $0200;

      Family            = $0F00;
      Model             = $00F0;
      Step              = $000F;

function CheckP5 : Word; far; external;

function GetP5Features : Word; far; external;

function GetP5Vendor : String; far; external;

function CheckP5_2 : Word; far; external;

{$L P5INFO}

const
     useSecondVersion : Boolean = False;

function GetFamily : Word;
 begin
  if useSecondVersion then
   GetFamily := (CheckP5_2 and Family) shr 8
  else
   GetFamily := (CheckP5 and Family) shr 8;
 end;

function GetModel : Word;
 begin
  if useSecondVersion then
   GetModel := (CheckP5_2 and Model) shr 4
  else
   GetModel := (CheckP5 and Model) shr 4;
 end;

function GetStep : Word;
 begin
  if useSecondVersion then
   GetStep := (CheckP5_2 and Step)
  else
   GetStep := (CheckP5 and Step);
 end;

procedure PrintBullet( doPrint : Word );
 begin
  if doPrint <> 0 then
   Write(#254)
  else
   Write(' ');
 end;

procedure PrintFeatures;
 var P5Features : Word;
 begin
  P5Features := GetP5Features;
  PrintBullet(P5Features and FPUonChip);
  WriteLn(' FPU on Chip');
  PrintBullet(P5Features and EnhancedV86);
  WriteLn(' Enhanced Virtual-8086 mode');
  PrintBullet(P5Features and IOBreakpoints);
  WriteLn(' I/O Breakpoints');
  PrintBullet(P5Features and PageSizeExtensions);
  WriteLn(' Page Size Extensions');
  PrintBullet(P5Features and TimeStampCounter);
  WriteLn(' Time Stamp Counter');
  PrintBullet(P5Features and ModelSpecificRegs);
  WriteLn(' Pentium processor-style model specific registers');
  PrintBullet(P5Features and MachineCheckExcept);
  WriteLn(' Machine Check Exception');
  PrintBullet(P5Features and CMPXCHG8B);
  WriteLn(' CMPXCHG8B Instruction');
  PrintBullet(P5Features and APIConChip);
  WriteLn(' APIC on chip');
end;

var C : Char;

begin
 WriteLn('P5Info/Pas  Version 1.04  Copyright(c) 1994,95 by B-coolWare.');
 WriteLn;
 if CheckP5 = 0 then
  begin
   Write('The CPU doesn''t seem to handle CPUID instruction. Give it a try anyway? [Y/N] ');
   repeat
    C := Upcase(ReadKey);
   until C in ['Y','N'];
   WriteLn(C,#10#13);
   if C = 'N' then
    Halt
   else
    if CheckP5_2 = $FFFF then
     begin
      WriteLn('Oops... No way, sorry...');
      Halt;
     end
    else
     useSecondVersion := True;
  end;

 WriteLn('Make ',GetP5Vendor);
 WriteLn('Family ',GetFamily,', Model ',GetModel,', Step ',GetStep);
 WriteLn;
 WriteLn('Processor Features:');
 PrintFeatures;
end.
