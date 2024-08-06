{$N-,E-,G+,S-,I-,V-,R-,X+}
uses CPUType;

function get486Model : Word; far; external;
{$L MODEL486 }

function HexW( W : Word ) : String;
 const
	D : array[0..15] of Char = '0123456789ABCDEF';
 begin
  HexW := D[Hi(W) shr 4] + D[Hi(W) and $0F] + D[Lo(W) shr 4] + D[Lo(W) and $0F];
 end;

var Id : Word;
    C  : Word;

const
    TestFailed : Boolean = False;

begin
 WriteLn('486 CPU Model Identifier  Version 1.01  Copyright(c) 1995 by B-coolWare.');
 WriteLn;
 C := CPU_Type;
 if (C > i286) and ((Hi(C) and 1) = 0) then { CPU is 386 or better in real mode }
  begin
   asm
	call	get486Model
	mov	Id,ax
	jz	@@CPUisOK
	mov	TestFailed,1
@@CPUisOK:
   end;
   if TestFailed then
    WriteLn('!!! CPU internal test FAILED or BIOS destroyed registers contents.');
   WriteLn('Code returned: ',HexW(Id));
   if (Id shr 8) and $0F = 4 then   { Intel or AMD 486 CPU }
    begin
     Write('CPU is 486');           { use Family field to determine CPU type }
     case (Id and $00F0) of	    { check Model field for known models }
      0,$10 : WriteLn('DX');
      $20   : WriteLn('SX');
      $30   : WriteLn('DX2/Overdrive');
      $40   : WriteLn('SL');
      $50   : WriteLn('SX2');
      $70   : WriteLn('DX2WB');
      $80   : WriteLn('DX4');
     else
      WriteLn('??');		   { unknown model code }
     end;
    end
   else
    WriteLn('This program cannot interprete returned code.');
  end
 else
  WriteLn('This program requires 386+ in Real mode...');
end.
