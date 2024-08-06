{$A-,B-,D+,E-,F+,G-,I-,L+,N-,O+,R-,S-,V-,X+}
{ --------------------------------------------------------------------------- }
{ CPUTYPE.PAS  TMi0SDGL(tm) Implementation module	       Version 1.17   }
{                                                                             }
{ Copyright(c) 1992-95 by B-coolWare.  Written by Bobby Z.                    }
{ --------------------------------------------------------------------------- }
{ files needed to build project:

  HEADER.ASH
  CPU_HL.ASM
  CPUSPEED.ASM
  CXMODEL.ASM
  P5INFO.ASM								      }

unit CPUType;

interface

const  i88     =  $0000;	{ Intel 8088 }
       i86     =  $0001;	{ Intel 8086 }
       V20     =  $0002;	{ NEC V20 }
       V30     =  $0003;	{ NEC V30 }
       i188    =  $0004;	{ Intel 80188 }
       i186    =  $0005;	{ Intel 80186 }
       i286    =  $0006;	{ Intel 80286 }
       i386sxr =  $0007;	{ Intel 80386sx real mode }
       i386sxv =  $0107;	{ Intel 80386sx V86 mode }
       i386dxr =  $0008;	{ Intel 80386dx real mode }
       i386dxv =  $0108;	{ Intel 80386dx V86 mode }
       i386slr =  $0009;        { IBM 80386SL real mode }
       i386slv =  $0109;        { IBM 80386SL V86 mode }
       i486sxr =  $000A;	{ Intel i486sx real mode }
       i486sxv =  $010A;	{ Intel i486sx V86 mode }
       i486dxr =  $000B;	{ Intel i486dx real mode }
       i486dxv =  $010B;	{ Intel i486dx V86 mode }
       c486slcr=  $000C;	{ Cyrix 486slc real mode }
       c486slcv=  $010C;	{ Cyrix 486slc V86 mode }
       c486r   =  $000D;	{ Cyrix 486sx/dx/dlc real mode }
       c486v   =  $010D;	{ Cyrix 486sx/dx/dlc V86 mode }
       i586r   =  $000E;	{ Intel Pentium real mode }
       i586v   =  $010E;	{ Intel Pentium V86 mode }
       cM1r    =  $000F;	{ Cyrix M1 (586) in real mode }
       cM1v    =  $010F;	{ Cyrix M1 (586) in V86 mode }
       iP24Tr  =  $0010;	{ Intel P24T in real mode }
       iP24Tv  =  $0110;	{ Intel P24T in V86 mode }
       ibm386r =  $0011;	{ IBM 386SLC in real mode }
       ibm386v =  $0111;	{ IBM 386SLC in V86 mode }
       ibm486r =  $0012;	{ IBM 486SLC in real mode }
       ibm486v =  $0112;	{ IBM 486SLC in V86 mode }
       ibm486r2=  $0013;	{ IBM 486SLC2 in real mode }
       ibm486v2=  $0113;	{ IBM 486SLC2 in V86 mode }
       umcU5sxr=  $0014;	{ UMC U5S }
       umcU5sxv=  $0114;
       umcU5dxr=  $0015;	{ UMC U5D }
       umcU5dxv=  $0115;
       am386sxr=  $0016;	{ AMD Am386SX }
       am386sxv=  $0116;
       am386dxr=  $0017;        { AMD Am386DX }
       am386dxv=  $0117;
       nx586r  =  $0018;	{ NexGen Nx586 in real mode }
       nx586v  =  $0118;	{ NexGen Nx586 in V86 mode }
       ibmBL3r =  $0019;	{ IBM 486BL3 Blue Lightning }
       ibmBL3v =  $0119;	
       am486dxr=  $001A;	{ AMD Am486DX?? }
       am486dxv=  $011A;
       iP54r   =  $001B;	{ Intel iP54 }
       iP54v   =  $011B;
       iP6r    =  $001C;	{ Intel P6 }
       iP6v    =  $011C;


{$F+}
function CPU_Type : Word;

function CPU_TypeStr : String;

function CoPro_TypeStr : String;

function CPUSpeed : Real;

function intCPUSpeed : Integer;

function isSMMAble : Boolean;

function checkEmu : Boolean;

function getCyrixModel : Word;

function CxStep : Byte;

function CxRevision : Byte;

function IsCyrix : Boolean;

const
	_CPU  : Byte = 0;
      FPUType : Byte = $FF;
{
     QEMMEntry: LongInt = 0;
     QEMMId   : array[0..8] of Char = 'QEMM386$'#0;
}

implementation

const 
      CPUFix  : LongInt = 0;
      Shift   : Word    = 2;

      umcU5s  = $14;
      umcU5d  = $15;

function CPU_Type; external;

function checkEmu; external;

function getCyrixModel; external;

function IsCyrix; external;
{$L CXMODEL}

{$IFDEF DPMI}
{$L CPU_TP.OBP}
{$ELSE}
{$L CPU_TP}
{$ENDIF}

function Speed(CPUId : Byte) : Word; external;
{$IFDEF DPMI}
{$L SPEED_TP.OBP}
{$ELSE}
{$L SPEED_TP}
{$ENDIF}

function CheckP5 : Word; far; external;

function GetP5Vendor : String; far; external;

function GetP5Features : Word; far; external;
{$L P5INFO}

function isSMMAble : Boolean; external;
{$L SMM}

function Vendor( CPU : Byte ) : String;
 begin
  case CPU of
   $0C,$0D,
   $0F     : Vendor := 'Cyrix Cx';
   $02,$03 : Vendor := 'NEC ';
   $09,$11,
   $12,$13,
   $19     : Vendor := 'IBM ';
   $14,$15 : Vendor := 'UMC ';
   $18     : Vendor := 'NexGen ';
   $1B     : Vendor := 'AMD Am';
   7,8     : if intCPUSpeed > 35 then
              Vendor := 'AMD Am'
             else
              Vendor := 'Intel ';
  else
   Vendor := 'Intel ';
  end;
 end;

procedure checkUMC;
 begin
  if _CPU >= i486sxr then
   if (CheckP5 and $0F00) = $400 then { Family = 4, don't care of other fields }
    if Pos('UMC',GetP5Vendor) <> 0 then
     if (GetP5Features and 1) = 1 then
      _CPU := umcU5d
     else
      _CPU := umcU5s;
 end;

function CxStep;
 begin
  CxStep := Hi(getCyrixModel) shr 4;
 end;

function CxRevision;
 begin
  CxRevision := Hi(getCyrixModel) and $0F;
 end;

function CxModel : String;
 begin
  case Lo(getCyrixModel) of
   0 : CxModel := 'Cyrix Cx486SLC';
   1 : CxModel := 'Cyrix Cx486DLC';
   2 : CxModel := 'Cyrix Cx486SL2';
   3 : CxModel := 'Cyrix Cx486DL2';
   4 : CxModel := 'Cyrix Cx486SR';
   5 : CxModel := 'Cyrix Cx486DR';
   6 : CxModel := 'Cyrix Cx486SR2';
   7 : CxModel := 'Cyrix Cx486DR2';
  $10: CxModel := 'Cyrix Cx486S';
  $11: CxModel := 'Cyrix Cx486S2';
  $12: CxModel := 'Cyrix Cx486SE';
  $13: CxModel := 'Cyrix Cx486S2E';
  $1A: begin
        CxModel := 'Cyrix Cx486DX';
        FPUType := FPUType and 1 + $10;
       end;
  $1B: begin
        CxModel := 'Cyrix Cx486DX2';
        FPUType := FPUType and 1 + $10;
       end;
  $30: begin
        CxModel := 'Cyrix M1 (586)';
        FPUType := FPUType and 1 + $10;
       end;
  $FE: CxModel := 'Texas Instruments Ti486SXL';
  else
   CxModel := 'Cyrix/Texas Instruments 486';
 end;
end;

function Am486Model : String;
 begin
  case CheckP5 and $00F0 of
   $0030 : Am486Model := '486DX2';
   $0070 : Am486Model := '486DX2+';
   $0080 : Am486Model := '486DX4';
   $0090 : Am486Model := '486DX4+';
  else
   Am486Model := '486';
  end;
 end;

function CPU_TypeStr;
 var CPU : Word;
begin
 CPU := CPU_Type;
 _CPU := CPU;
 checkUMC;
 if (_CPU = $0A) and (CheckP5 <> 0) then
  begin
   case CheckP5 and $00F0 of
    $0000,$0010 : CPU_TypeStr := 'Intel i486DX';
          $0020 : CPU_TypeStr := 'Intel i486SX';
          $0030 : CPU_TypeStr := 'Intel i486DX2/OverDrive';
          $0040 : CPU_TypeStr := 'Intel i486SL';
          $0050 : CPU_TypeStr := 'Intel i486SX2';
          $0070 : CPU_TypeStr := 'Intel i486DX2WB (P24D)';
          $0080 : CPU_TypeStr := 'Intel i486DX4';
    else
     CPU_TypeStr := 'Intel i486??';
   end;
   exit;
  end;
 case CPU of
  i88      : CPU_TypeStr := Vendor(CPU)+'8088';
  i86      : CPU_TypeStr := Vendor(CPU)+'8086';
  i188     : CPU_TypeStr := Vendor(CPU)+'80188';
  i186     : CPU_TypeStr := Vendor(CPU)+'80186';
  v20      : CPU_TypeStr := Vendor(CPU)+'V20';
  v30      : CPU_TypeStr := Vendor(CPU)+'V30';
  i286     : CPU_TypeStr := Vendor(CPU)+'80286';
  i386sxr,
  i386sxv  : CPU_TypeStr := Vendor(CPU)+'386SX';
  i386slr,
  i386slv  : CPU_TypeStr := Vendor(CPU)+'80386SL';
  i386dxr,
  i386dxv  : CPU_TypeStr := Vendor(CPU)+'386DX';
  i486sxr,
  i486sxv  : CPU_TypeStr := Vendor(CPU)+'i486SX';
  i486dxr,
  i486dxv  : CPU_TypeStr := Vendor(CPU)+'i486DX or i487SX';
  c486slcr,
  c486slcv,
  c486r,
  c486v    : CPU_TypeStr := CxModel;
  i586r,
  i586v    : CPU_TypeStr := Vendor(CPU)+'Pentium';
  cM1r,
  cM1v	   : CPU_TypeStr := Vendor(CPU)+'M1 (586)';
  umcU5sxr,
  umcU5sxv : CPU_TypeStr := Vendor(CPU)+'U5-S';
  umcU5dxr,
  umcU5dxv : CPU_TypeStr := Vendor(CPU)+'U5-D';
  iP24Tr,
  iP24Tv   : CPU_TypeStr := Vendor(CPU)+'iP24T (Pentium OverDrive)';
  ibm386r,
  ibm386v  : CPU_TypeStr := Vendor(CPU)+'386SLC';
  ibm486r,
  ibm486v  : CPU_TypeStr := Vendor(CPU)+'486SLC';
  ibm486r2,
  ibm486v2 : CPU_TypeStr := Vendor(CPU)+'486SLC2';
  ibmBL3r,
  ibmBL3v  : CPU_TypeStr := Vendor(CPU)+'486BL3 (Blue Lightning)';
  iP54r,
  iP54v    : CPU_TypeStr := Vendor(CPU)+'iP54';
  am486dxr,
  am486dxv : CPU_TypeStr := Vendor(CPU)+Am486Model;
  nx586r,
  nx586v   : CPU_TypeStr := Vendor(CPU)+'Nx586';
  iP6r,
  iP6v	   : CPU_TypeStr := Vendor(CPU)+'iP6';
 end;
end;

function CoPro_TypeStr;
 var C : Word;
     S : String;
 begin
  if FPUType = $FF then
   begin
    C := CPU_Type;
    case C of
     c486slcr,
     c486slcv,
     c486r,
     c486v : CxModel;
    end;
   end;
  case FPUType of
   0,1     : S := 'Unknown';
   2       : S := 'None';
   3       : S := 'Weitek';
   4       : S := 'Intel 8087';
   5       : S := 'Intel 8087 and Weitek';
   6       : S := 'Intel i487sx';
   7       : S := 'Intel i487sx and Weitek';
   8       : S := 'Intel 80287';
   9       : S := 'Intel 80287 and Weitek';
   $A      : S := 'Cyrix 82x87';
   $B      : S := 'Cyrix 82x87 and Weitek';
   $C      : S := 'Intel 80387';
   $D      : S := 'Intel 80387 and Weitek';
   $E      : S := 'Cyrix 83x87';
   $F      : S := 'Cyrix 83x87 and Weitek';
   $10     : S := 'Internal';
   $11     : S := 'Internal and Weitek';
   $12     : S := 'Cyrix 84x87';
   $13     : S := 'Cyrix 84x87 and Weitek';
   $14     : S := 'Intel 80287XL';
   $15     : S := 'Intel 80287XL and Weitek';
   $16	   : S := 'IIT 2C87';
   $17     : S := 'IIT 2C87 and Weitek';
   $18	   : S := 'IIT 3C87';
   $19     : S := 'IIT 3C87 and Weitek';
   $1A	   : S := 'ULSI 83x87';
   $1B     : S := 'ULSI 83x87 and Weitek';
   $1C	   : S := 'Cyrix EMC87';
   $1D     : S := 'Cyrix EMC87 and Weitek';
   $1E	   : S := 'C&T 38700';
   $1F     : S := 'C&T 38700 and Weitek';
   $20     : S := 'NexGen Nx587';
   $21     : S := 'NexGen Nx587 and Weitek';
   $22     : S := 'IIT 4C87';
   $23     : S := 'IIT 4C87 and Weitek';
  else
   S := 'Unknown';
  end;
  if (C >= i286) and checkEmu then
   S := S+', Emulated';
  CoPro_TypeStr := S;
 end;

function CPUSpeed;
 var W : Word;
 begin
  W := Speed(_CPU);
  CPUSpeed := ((LongInt(Shift)*CPUFix)/W+5)/10;
 end;

function intCPUSpeed;
 var W : Word;
 begin
  W := Speed(_CPU);
  intCPUSpeed := ((LongInt(Shift)*CPUFix) div W + 5) div 10;
 end;

end.
