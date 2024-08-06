; -----------------------------------------------------------------------------
; P5INFO.ASM  Pentium Processor Feature Information		  Version 1.03
;
; Copyright(c) 1994,95 by B-coolWare.  Written by Bobby Z.
; This code is part of TMi0SDGL(tm) CPU/FPU Feature Detection Library.
; -----------------------------------------------------------------------------
; These routines also work on new Intel and non-Intel 386 and 486 chips.
; You should check for 5 in chip family field to assure this is really P5
; and for GetVendor = 'GenuineIntel' to assure it is from Intel.
;

	INCLUDE	HEADER.ASH

	.CODE

	PUBLIC	CheckP5, GetP5Features, GetP5Vendor, CheckP5_2

; GetP5Features returns word with following bitfields:

FPUonChip		equ	0000000001b
EnhancedV86		equ	0000000010b
IOBreakPoints		equ	0000000100b
PageSizeExtensions	equ	0000001000b
TimeStampCounter	equ	0000010000b
ModelSpecificRegisters	equ	0000100000b
MachineCheckException	equ	0010000000b
CMPXCHG8BInstruction	equ    	0100000000b
APIConChip		equ	1000000000b

; CheckP5 returns word with following bitfields:

chipInfo	record chipFamily:4, chipModel:4, chipStep:4

EF_ID	equ	00200000h	; ID flag in EFLAGS

IF ??version LT 400h

cpuid	equ	<db 0Fh,0A2h>	; P5 info instruction, also handled by new
				; 486's (and 386's?)
ENDIF

CheckP5		proc
; checks if P5's cpuid instruction will work ok.
	mov	ax,sp
	push	sp
	pop	bx
	cmp	bx,ax
	jnz	@@noP5
	mov	ax,7000h
	pushf
	push	ax
	popf
	pushf
	pop	ax
	popf
	and	ax,7000h
	jz	@@noP5
	.386
	pushfd
	pop	eax
	mov	ecx,eax
	xor	eax,EF_ID
	push	eax
	popfd
	pushfd
	pop	eax
	push	ecx
	popfd
	and	eax,EF_ID
	and	ecx,EF_ID
	cmp	eax,ecx
	jz	@@noP5
	clr	eax
	inc	al		; eax = 1, get chip model and features
IF ??version GE 400h
	.586
ENDIF
	cpuid
	jmp	@@Q
	.8086
@@noP5:
	clr	ax
@@Q:
	ret
	endp

trap06:
	add	sp,4
	sub	ax,ax
	dec	ax
	popf
	jmp	afterTrap

CheckP5_2	proc
; invokes CPUID without checking for ID flag
	mov	ax,sp
	push	sp
	pop	bx
	cmp	bx,ax
	jnz	noP5
	mov	ax,7000h
	pushf
	push	ax
	popf
	pushf
	pop	ax
	popf
	and	ax,7000h
	jz	noP5
	.386
	mov	ax,3506h
	int	21h
	push	ds
	push	es
	push	bx
	push	ds
	mov	ax,cs
	mov	ds,ax
	mov	dx,offset trap06
	mov	ax,2506h
	int	21h
	pop	ds
	clr	eax
	inc	al		; eax = 1, get chip model and features
IF ??version GE 400h
	.586
ENDIF
	cpuid
afterTrap:
	pop	dx
	pop	ds
	push	ax
	mov	ax,2506h
	int	21h
	pop	ax
	pop	ds
	jmp	Q
	.8086
noP5:
	clr	ax
	dec	ax
Q:
	ret
	endp


GetP5Features	proc
; returns features word
	call	CheckP5
	or	ax,ax
	jz	@@Q
	xchg	dx,ax
@@Q:
	ret
	endp

	.8086

GetP5Vendor	proc Result : dword
; fills 13-byte buffer at [Result] with Id string

	les	di,Result
	call	CheckP5
	or	ax,ax
	jnz	@@GetInfo
	stosb
	jmp	@@Q
@@GetInfo:
	.386
	clr	eax		; get vendor id in [ebx,edx,ecx]
IF ??version GE 400h
	.586
ENDIF
	cpuid
	mov	al,12
	stosb
	xchg	ebx,eax
	stosd
	xchg	edx,eax	
	stosd
	xchg	ecx,eax
	stosd
@@Q:
	.8086
	ret
	endp

	END
