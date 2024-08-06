;------------------------------------------------------------------------------
; LSTRING.ASH  Lstring definition macro. Defines a string in Pascal format:
;              <Length>,<string data>[,CR[,LF]]. Such string can be an input
;	       to WriteStr routine defined in DOSINOUT.ASH.
;
; Copyright(c) 1992,95 by B-coolWare.  Written by Bobby Z.
;------------------------------------------------------------------------------

CrLf	EQU	1
CrOnly	EQU	2

__Lstring__	equ	1

LString	macro Name,String,CrLfStat
;; Parameters: PublicName, String data, Add CR/LF or CR at the end (optional).

	MASM51
L_&Name	SIZESTR <&String>	;; L_Name will be = length(String)

if CrLfStat
 if CrLfStat EQ CrLf
  L_&Name = L_&Name+2		;; if insert CR/LF then length := length+2
 else
  if CrLfStat EQ CrOnly
   L_&Name = L_&Name+1		;; if insert CR only then length := length+1
  endif 
 endif
endif

Name	db	 L_&Name, '&String'

if CrLfStat 
 if CrLfStat EQ CrLf
	db	13,10		;; LF,CR
 else
  if CrLfStat EQ CrOnly
	db	13		;; CR
  endif
 endif
endif
	MASM
	endm
