; -----------------------------------------------------------------------------
; CPUSPEED.ASM  CPU speed measurement routine			Version 1.17
;
; Too-Much-In-One-So-Don't-Get-Lost(tm) CPU/FPU feature detection library
;
; Copyright(c) 1993-95 by B-coolWare.  Written by Bobby Z.
; -----------------------------------------------------------------------------

	INCLUDE	HEADER.ASH

	.DATA

	EXTRN	CPUFix: DWORD		; CPU Type speed fix constant
	EXTRN	Shift : WORD		; current loop length

	.CODE

ifndef	__use_386__
 ifdef  __use_286__
	.286
 endif
else
 __use_286__	equ	1
	.386
endif

	PUBLIC	Speed   

_bpes	equ	<byte ptr es:>
_wp	equ	<word ptr>

clr	macro	reg
	sub	reg,reg
	endm

IsUnderWin	proc near
	mov	ax,1600h
	int	2Fh
	or	al,al
	jz	@@nowin
	cmp	al,80h
	jz	@@nowin
	stc
	ret
@@nowin:
	clc
	ret
	endp


WinStartCritical	proc near
	push	ax
	call	IsUnderWin
	jnc	@@Q
	mov	ax,1681h
	int	2Fh
@@Q:
	pop	ax
	ret
	endp

WinEndCritical		proc near
	push	ax
	call	IsUnderWin
	jnc	@@Q
	mov	ax,1682h
	int	2Fh
@@Q:
	pop	ax
	ret
	endp

isUnderDV	proc near
	mov	ax,2B01h
	push	cx dx
	mov	cx,4445h
	mov	dx,5351h
	int	21h
	pop	dx cx
	cmp	al,0FFh
	jz	@@noDV
	stc
	ret
@@noDV:
	clc
	ret
	endp

DVStartCritical	proc near
	push	ax
	call	isUnderDV
	jnc	@@Q
	mov	ax,101Bh
	int	15h
@@Q:
	pop	ax
	ret
	endp

DVEndCritical	proc near
	push	ax
	call	isUnderDV
	jnc	@@Q
	mov	ax,101Ch
	int	15h
@@Q:
	pop	ax
	ret
	endp


; -----------------------------------------------------------------------
; Speed modifies CPUFix and Shift and returns value which should b used
; as follows:
;
; CPUSpeed_In_MHz := (((CPUFix*Shift)/Speed)+5)/10;
;
; This formulae is taken from Norton SysInfo unchanged. I've no idea why
; computations done this way, but it works ok.
; Resulting value may be rounded to nearest integer, or may be left unchanged
; if you want exact CLK frequency value.

; function Speed( _CPUId : Byte ) : Word;
; unsigned int Speed( unsigned short _CPUId );

; _CPUId is value from 0 to 13h identifying processor we're running on
; 8088   = 00h
; 8086   = 01h
; V20    = 02h
; V30    = 03h
; 188    = 04h
; 186    = 05h
; 286    = 06h
; 386sx  = 07h
; 386dx  = 08h
; 386sl  = 09h
; 486sx  = 0Ah
; 486dx  = 0Bh
; 486slc = 0Ch
; Cx486  = 0Dh
; P5     = 0Eh
; CxM1	 = 0Fh
; P24D	 = 10h
; 386SLC = 11h
; 486SLC = 12h
; 486SLC2= 13h
; UMC U5S= 14h
; UMC U5D= 15h
; Am386SX= 16h
; Am386DX= 17h
; iP6	 = 18h

MaxCPUid	equ	18h	; change this if you added new CPU types

Speed	PROC
ARG	_CPUId:BYTE
ifdef	__DPMI__
	mov	ax,dpmiCreateCodeAlias
	mov	bx,cs
	int	31h
	mov	es,ax
else
	push	cs
	pop	es
endif
	cmp	_CPUId,MaxCPUid	; checking if we know this CPU
	jbe	@@okcpu
	mov	_CPUId,MaxCPUid	; fixing CPU id - let's assume it's P5 or
				; better :)
@@okcpu:
	mov	cx,2
	mov	_bpes[Indic],0
	call	WinStartCritical
	call	DVStartCritical
@@1:
	mov	Shift,cx
	call	Speed_Test
	cmp	ax,1000h
	jnb	@@2
	mov	cx,Shift
	shl	cx,1
	shl	cx,1
	shl	cx,1
	jmp	@@1
@@2:
	push	ax
	mov	cx,Shift
	mov	_bpes[Indic],1
	call	Speed_Test
	call	WinEndCritical
	call	DVEndCritical
	pop	dx
	sub	dx,ax
	xchg	ax,dx
ifdef	__use_386__
	movzx	bx,_CPUId
else
	mov	bl,_CPUId
	clr	bh
endif
ifdef	__use_286__
	shl	bx,2
else
	shl	bx,1
	shl	bx,1
endif
ifdef	__use_386__
	mov	edx,es:CPUFixes[bx]
	mov	CPUFix,edx
else
	mov	dx,_wp es:CPUFixes[bx]
	mov	_wp CPUFix,dx
	mov	dx,_wp es:CPUFixes[2][bx]
	mov	_wp CPUFix[2],dx
endif
ifdef	__DPMI__
	push	ax bx
	mov	ax,dpmiFreeLDTDesc
	mov	bx,es
	int	31h
	pop	bx ax
endif
	ret
	ENDP

Speed_Test	PROC	NEAR
; returns number of tick-tacks spent performing known instruction?
; or difference between time taken by plain group of similar instructions and
; time taken by loop of that instructions? I dinna find out what exactly it
; does, but it works - what else do i (and u) need? 

	PUSH	SI DI
	CLR	DX,DX
	MOV	SI,0AAAAH
	MOV	BX,05555H
	IN	AL,61h
	JMP	$+2
	AND	AL,0FCH
	OUT	61h,AL
	JMP	$+2
	MOV	AL,0B4h
	OUT	43h,AL
	JMP	$+2
	CLR	AL
	OUT	42h,AL
	JMP	$+2
	OUT	42h,AL
	JMP	$+2
	IN	AL,61h
	MOV	DI,AX
	OR	AL,01
	CMP	_BPES[INDIC],0
	JZ	@@1
	JMP	@@2
@@1:
	CLI
	OUT	61h,AL
@@3:
Sprite	equ	<8Bh, 0C6h, 0F7h, 0F3h>
;	MOV	AX,SI
;	DIV	BX
	db	101	dup(Sprite)
	DEC	CX
	JZ	@@4
	JMP	@@3
@@2:
	CLI
	OUT	61h,AL
	NOP			; well, maybe this is the right place for
				; NOP, so i left it here. Try to kill it and
				; see what will happen...
@@5:
	db	Sprite
	DEC	CX
	JZ	@@4
	JMP	@@5

@@4:
	MOV	AX,DI
	OUT	61h,AL
	JMP	$+2
	STI
	IN	AL,42h
	JMP	$+2
	XCHG	AH,AL
	IN	AL,42h
	JMP	$+2
	XCHG	AH,AL
	NEG	AX
	PUSH	AX
	IN	AL,61h
	JMP	$+2
	AND	AL,0FDh
	OUT	61h,AL
	POP	AX DI SI
	RET
	ENDP
Indic	db	?

CPUFixes:

; For on different processors the same instruction takes different number of 
; CPU clocks, we should adjust absolute timer value using our knowledge of
; known CPU's timings.

	dd	2AD26h		; 8088
	dd	2AD26h		; 8086
	dd	0E90Bh		; V20
	dd	0DFB9h		; V30
	dd	0BA6Fh		; 188
	dd	0BA6Fh		; 186
	dd	06FDCh		; 286
	dd	07480h		; 386sx
	dd	07480h		; 386dx
	dd	07480h		; 386sl
	dd	07480h		; 486sx
	dd	07480h		; 486dx
	dd	0668Ah		; Cx486slc
	dd	0668Ah		; Cx486???
	dd	07850h		; Pentium
	dd	0668Ah		; CxM1
	dd	07850h		; P24T
	dd	07486h		; IBM 386SLC
	dd	07486h		; IBM 486SLC	???
	dd	07486h		; IBM 486SLC2	???
	dd	03C90h		; UMC U5-S
	dd	03C90h		; UMC U5-D
	dd	07415h		; AMD Am386SX
	dd	07415h		; AMD Am386DX
	dd	03079h		; NexGen Nx586 	!!! needs adjustment
	dd	07486h		; IBM 486BL3	???
	dd	07480h		; AMD Am486	!!! needs adjustment
	dd	07850h		; P54
	dd	0D28Ch		; Intel P6	!!! needs adjustment

	END
