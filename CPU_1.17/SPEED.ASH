; -----------------------------------------------------------------------------
; SPEED.ASH  CPU speed measurement routine for Assembler         Version 1.17
;
; Too-Much-In-One-So-Don't-Get-Lost(tm) CPU/FPU feature detection library
;
; Copyright(c) 1993-95 by B-coolWare.  Written by Bobby Z.
; -----------------------------------------------------------------------------

	LOCALS	@@

CPUFix	dd	?	; CPU Type speed fix constant
Shift 	dw	?	; current loop length

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

MaxCPUid	equ	18h	; change this if you added new CPU types

Speed	PROC	NEAR
ARG	_CPUid:BYTE
	push	cs
	pop	es
	cmp	_CPUid,MaxCPUid	; checking if we know this CPU
	jbe	@@okcpu
	mov	_CPUid,MaxCPUid	; fixing CPU id - let's assume it's P5 or
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
	mov	bl,_CPUid
	clr	bh
	shl	bx,1
	shl	bx,1
	mov	dx,_wp es:CPUFixes[bx]
	mov	_wp CPUFix,dx
	mov	dx,_wp es:CPUFixes[2][bx]
	mov	_wp CPUFix[2],dx
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

intCPUSpeed	proc	near
	push	ax
	call	Speed
	push	ax
	mov	dx,word ptr CPUFix[2]
	mov	ax,word ptr CPUFix
	mov	cx,Shift
	mul	cx
	pop	cx
	div	cx
	add	ax,5
	mov	cl,10
	div	cl		; al = CPU MHz
	ret
	endp

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
	dd	03079h		; NexGen Nx586	!!! needs adjustment
	dd	07486h		; IBM 486BL3	!!! needs adjustment
	dd	07480h		; AMD Am486	!!! needs adjustment
	dd	07850h		; P54
	dd	0D28Ch		; Intel P6	!!! needs adjustment

