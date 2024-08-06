{$A+,B-,D+,E-,F-,G-,I-,L+,N-,O-,R-,S-,V-,X+}
{$M 16384,0,655360}
{ --------------------------------------------------------------------------- }
{ CPU.PAS  Sample program demonstrating usage of TMi0SDGL(tm) routines        }
{									      }
{ Copyright(c) 1994,95 by B-coolWare.  Written by Bobby Z.		      }
{ --------------------------------------------------------------------------- }
{ files needed to build project:

  HEADER.ASH
  CPU_HL.ASM
  CPUSPEED.ASM
  SMM.ASM
  CPUTYPE.PAS								      }

uses CPUType;

const
	SMM : array[Boolean] of String[16] = 
              ('','  [SL Enhanced]');

begin
 WriteLn('CPU Type Identifier/Pas  Version 1.17  Copyright(c) 1992-95 by B-coolWare.');
 WriteLn;
 WriteLn('  Processor: ',CPU_TypeStr, ', ',intCPUSpeed,'MHz',SMM[isSMMAble]);
 if IsCyrix then
  WriteLn('  Cyrix CPU Step: ',CxStep,', Revision: ',CxRevision);
 WriteLn('Coprocessor: ',CoPro_TypeStr);
end.
