; -----------------------------------------------------------------------------
; CPU_HL.ASM   CPU Type detection routine for hi-level languages Version 1.17
;
; Too-Much-In-One-So-Don't-Get-Lost(tm) CPU/FPU feature detection library
;
; Copyright(c) 1992-95 by B-coolWare.  Written by Bobby Z.
; -----------------------------------------------------------------------------
; This is a port from CPU_TYPE.ASH - the assembler version of TMi0SDGL(tm)
; See file CPU_TYPE.ASH for extensive comments and all.

	INCLUDE	HEADER.ASH

	.DATA

	EXTRN FPUType:BYTE

	.CODE

	PUBLIC	CPU_Type
	PUBLIC	checkEmu

; TP/BP:
; function CPU_Type : Word;
;
; C/C++:
; word CPU_Type(void);
;
; returns current CPU code (see CPUTYPE.PAS or CPUTYPE.H for details) and sets
; current FPU code in FPUType variable.

EF_AC		equ	00040000h	; AC bit in EFLAGS register
EF_ID		equ	00200000h	; ID bit in EFLAGS register
MSW_NE		equ	00000020h	; NE bit in MSW register

IF ??version LT 400h

cpuid		equ	<db 0Fh,0A2h>	; new i486 & Pentium instruction

ENDIF

	.8086
CPU_Type	proc
LOCAL	hasFPUonChip:BYTE
	mov	hasFPUonChip,0
	push	bx cx si
	sub	bx,bx
	push	sp
	pop	ax
	cmp	ax,sp
	jnz	@@Ct_000
	mov	ax,7000h
	pushf
	push	ax
	popf
	pushf
	pop	ax
	popf
	mov	bl,6
	and	ax,7000h
	jz	@@Ct_200
	inc	bx

	.386p
	clr	si
	mov	eax,cr0
	mov	ecx,eax
	xor	eax,10h
	mov	cr0,eax
	mov	eax,cr0
	mov	cr0,ecx
	xor	eax,ecx
	jz	@@L100
	inc	si

@@L100:
	.386

	mov	ax,sp
	and	sp,0FFFCh
	pushfd
	pushfd
	pop	edx
	mov	ecx,edx
	xor	edx,EF_AC
	and	ecx,EF_AC
	push	edx
	popfd
	pushfd
	pop	edx
	popfd
	mov	sp,ax
	and	edx,EF_AC
	cmp	edx,ecx
	jnz	@@486
	or	si,si
	jz	@@386dx
	inc	bx
	jmp	@@386dx

@@486:
	mov	bx,0Ah
	push	bx
	mov	bx,100h
	mov	ah,7Fh
	cwd
	sahf
	div	bx
	lahf
	xor	al,ah
	and	al,0D5h
	pop	bx
	jnz	@@586
	inc	bx
	inc	bx
	or	si,si
	jz	@@586
	inc	bx
@@586:

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
	jz	@@486sdx
	clr	eax
	inc	al
	push	bx
IF ??version GE 400h
	.586
ENDIF
	cpuid
	pop	bx
	and	dl,1
	mov	hasFPUonChip,dl
	and	ah,0Fh
	cmp	ah,4
	jb	@@386dx
	je	@@486sdx2
	push	bx
	push	ax
	clr	eax
	cpuid
	pop	ax
	cmp	ebx,'iryC'
	pop	bx
	jnz	@@P5
	cmp	edx,'snIx'
	jnz	@@P5
	cmp	ecx,'daet'
	jnz	@@P5
	mov	bl,0Fh		; CxM1
	jmp	@@L1	
@@P5:
	mov	bl,ah
	cmp	bl,6
	jz	@@P6
	and	ax,10F0h
	cmp	ah,10h
	jz	@@P24T
	cmp	al,20h
	mov	bl,1Bh
	jz	@@L1
	mov	bl,0Eh
	jmp	@@L1
@@P24T:
	mov	bl,10h
	jmp	@@L1
@@P6:
	mov	bl,1Ch
	jmp	@@L1
@@386dx:
	call	checkIBMmsrs
	jmp	@@L1
@@486sdx2:
	push	bx
	clr	eax
	cpuid
	cmp	ebx,'htuA'
	pop	bx
	jnz	@@notAMD
	cmp	edx,'itne'
	jnz	@@notAMD
	cmp	ecx,'DMAc'
	jnz	@@notAMD
	mov	bl,1Ah
	jmp	@@L1
@@notAMD:	
	call	checkIBMmsrs3
	jc	@@L1
	mov	bl,0Ah
	push	bx
	clr	eax
	inc	al
IF ??version GE 400h
	.586
ENDIF
	cpuid
	pop	bx
	test	al,al
	jnz	@@setNibble
	inc	al
@@setNibble:
	or	bh,al
	jmp	@@L1

@@486sdx:

	.486p
	call	isInOSZwei
	jnz	@@L1
	cmp	bl,0Ch
	jae	@@L1
	mov	eax,cr0
	mov	ecx,eax
	db	66h,83h,0E0h,0DFh
	mov	cr0,eax
	mov	eax,cr0
	cmp	eax,ecx
	jnz	@@486dx
	or	eax,MSW_NE
	mov	cr0,eax
	mov	eax,cr0	; this instruction causes the host to clear EBX  ???!!!
	cmp	eax,ecx
	jnz	@@486dx
	dec	bx
@@486dx:
	inc	bx
	mov	eax,ecx
	mov	cr0,eax
	call	checkIBMmsrs3

@@L1:
	cmp	bl,07h
	jb	@@L101
	cmp	bl,08h
	ja	@@L101
	mov	ax,5555h
	xor	dx,dx
	mov	cx,2
	div	cx
	jnz	@@L101
	mov	bl,18h
@@L101:
	.286p
	smsw	ax
	and	al,1
	or	bh,al
	jmp	@@Ct_200
@@Ct_000:
	mov	bl,4
	mov	cl,33
	clr	ax
	dec	ax
	shl	ax,cl
	jnz	@@Ct_100
	mov	bl,2
	clr	cx
	dec	cx
	db	0F3h,26h,0ACh
	jcxz	@@Ct_100
	clr	bx
@@Ct_100:
	clr	ax
	call	Test_Buffer
	jcxz	@@Ct_200
	inc	bx
@@Ct_200:
	mov	dl,10h
	cmp	hasFPUonChip,1
	jz	@@noCheck
@@checkFPU:
	call	FPU_Type
@@noCheck:
	call	checkWeitek
	mov	FPUType,dl
	mov	ax,bx
	pop	si cx bx
	.8086
	ret
	endp

	db	13,10
	db	'				Too much is not enough...',13,10
	db	'            				(Deep Purple)',13,10
	db	13,10
	db	'TMi0SDGL(tm) CPU/FPU feature detection library  Version 1.17',13,10
	db	'Copyright(c) 1992-95 by B-coolWare.     Released as freeware.',13,10

Test_Buffer	proc near
; this routine will never be called in DPMI applications for they won't run
; on CPU less that 286. Thus we need no precautions about writing to code
; segment.
	push	es di
	std
	mov	_bpcs[@@0],41h
	push	cs
	pop	es
	ldi	@@2
	mov	al,_bpcs[@@1]
	mov	cx,3
	cli
	rep	stosb
	cld
	nop
	nop
	nop
@@0:	inc	cx
@@1:
	sti
@@2:	
	sti
	pop	di es
	ret
	endp

	.386p

IF ??version LT 400h

rdmsr	macro	Reg
	mov	ecx,Reg
	db	0Fh,32h
	endm
wrmsr	macro	Reg
	mov	ecx,Reg
	db	0Fh,30h
	endm

ENDIF

checkIBMmsrs	proc	near
; we'll get here only on 386 with CPUID support
; returns CF=1 if IBM MSRs found and sets BL in this case
	push	dx
	smsw	ax
	test	al,1		; we can only do this nasty code in real mode...
	jnz	@@Qs1

;	jz	@@OkWRMSR
;	call	checkQEMM	; ...or under QEMM 7.04 and up.
;	jnc	@@Qs1
;@@OkWRMSR:
	push	bx
	push	ds
	mov	ax,350Dh
	int	21h
	push	es
	push	bx
	push	cs
	pop	ds
	mov	dx,offset @@trap0D
	mov	ax,250Dh	; intercept GPF exception
	int	21h
	mov	ax,3506h
	int	21h
	push	es
	push	bx
	mov	dx,offset @@trap06
	mov	ax,2506h	; and Invalid Instruction exception
	int	21h
IF ??version LT 400h
	rdmsr	1000h		; we'll get Exception 13 if this MSR is not 
ELSE
	.586p
	mov	ecx,1000h
	rdmsr
ENDIF
	stc			; valid or Exception 06 if this is not IBM386.
	mov	bl,11h		; IBM 386SLC
	jmp	@@Qs
@@trap06:
	add	sp,4
	popf
	clc
	jmp	@@Qs
@@trap0D:
	add	sp,4
	popf
	clc
@@Qs:
	pop	dx
	pop	ds
	pushf
	mov	ax,2506h
	int	21h
	popf
	pop	dx
	pop	ds
	pushf
	mov	ax,250Dh
	int	21h
	popf
	pop	ds
	pop	bx
@@Qs1:
	pop	dx
	ret
	endp

checkIBMmsrs2	proc	near
; we'll get here only on 486 with CPUID support
	push	dx
	call	checkIBMmsrs	; first check if this is IBM 486SLC chip
	jnc	@@Qi
	push	bx
	push	ds
	mov	ax,350Dh
	int	21h
	push	es
	push	bx
	push	cs
	pop	ds
	mov	dx,offset @@trap0D2
	mov	ax,250Dh
	int	21h
IF ??version LT 400h
	rdmsr	1002h		; try to read 486SLC2 specific MSR
ELSE
	.586p
	mov	ecx,1002h
	rdmsr
ENDIF
	mov	bl,13h		; IBM 486SLC2
	jmp	@@Qi2
@@trap0D2:
	add	sp,4
	popf
	stc
	mov	bl,12h		; IBM 486SLC
@@Qi2:
	pop	dx
	pop	ds
	pushf
	mov	ax,250Dh
	int	21h
	popf
	pop	bx
@@Qi:
	pop	dx
	ret
	endp

checkIBMmsrs3	proc	near
; we'll get here only on 486 with CPUID support
	push	dx
	call	checkIBMmsrs2	; first check if this is IBM 486SLC chip
	jnc	@@Qi4
	push	bx
	push	ds
	mov	ax,350Dh
	int	21h
	push	es
	push	bx
	push	cs
	pop	ds
	mov	dx,offset @@trap0D3
	mov	ax,250Dh
	int	21h
IF ??version LT 400h
	rdmsr	1004h		; try to read 486BL3 specific MSR
ELSE
	.586p
	mov	ecx,1004h
	rdmsr
ENDIF
	mov	bl,19h		; IBM 486BL3
	jmp	@@Qi3
@@trap0D3:
	add	sp,4
	popf
	stc
@@Qi3:
	pop	dx
	pop	ds
	pushf
	mov	ax,250Dh
	int	21h
	popf
	pop	bx
@@Qi4:
	pop	dx
	ret
	endp


	.8086

isInOSZwei	proc	near

	push	ax bx
	mov	ax,4010h
	int	2Fh
	cmp	ax,4010h
	pop	bx ax
	ret
	endp

checkWeitek	proc	near
	cmp	bl,7
	jb	@@1
	.386
	clr	eax
	int	11h
	test	eax,1000000h
	.8086
	jz	@@1
	or	dl,1
@@1:
	ret
	endp

	.8086

FPU_Type	proc near
	.286p
	.287
	push	ds
ifdef	__DPMI__
	push	ax bx
	mov	ax,dpmiCreateCodeAlias
	mov	bx,cs
	int	31h
	mov	ds,ax
	pop	bx ax
else
	push	cs
	pop	ds
	ASSUME  DS:@code
endif
	mov	dl,2
	fninit
	xor	cx,cx
	jmp	$+2
	mov	ds:fpudata1,5A5Ah
	fnstsw	ds:fpudata1
	mov	ax,ds:fpudata1
	or	al,al
	jnz	@@L161
	fnstcw	ds:fpudata1
	mov	ax,ds:fpudata1
	and	ax,103Fh
	cmp	ax,3Fh
	jne	@@L161
	mov	dl,4
	fstenv	ds:fpudata3
	wait
	and	ds:fpudata1,0FF7Fh
	fldcw	ds:fpudata1
	fdisi
	fstcw	ds:fpudata1
	wait
	test	ds:fpudata1,80h
	jnz	@@L161
	mov	dl,8
	fninit
	fld1
	fldz
	fdivp	st(1),st
	fld	st
	fchs
	fcompp
	fstsw	ds:fpudata1
	wait
	mov	ax,ds:fpudata1
	sahf
	jz	@@L161
	mov	dl,0Ch
	cmp	bl,09h
	jae	@@50
	cmp	bl,06h
	jb	@@50

@@checkIIT:
	fld	ds:[denormal]
	fadd	st(0),st
	fnstsw	ax
	test	al,02h
	jnz	@@50
	cmp	dl,0Ch
	jz	@@300
	mov	dl,22
	jmp	@@L161
@@300:
	cmp	bl,0Ah
	jb	@@301
	mov	dl,22h
	jmp	@@L161
@@301:
	mov	dl,24
	jmp	@@L161
@@50:
	fninit
	fldpi
	f2xm1
	fstp	ds:fpudata2
	wait
	cmp	_wp ds:[fpudata2+2],3FC9h
	jne	@@L15
	or	dl,2
@@L15:
	fninit
	fldcw	ds:_53bit_prec
	fld	tbyte ptr ds:op1
	fld1
	faddp	st(1),st
	fstp	ds:tera
	fnstsw	ax
	wait
	test	al,20h
	jnz	@@L16
	cmp	byte ptr ds:tera,0F8h
	jnz	@@L16
	cmp	byte ptr ds:tera[9],40h
	jnz	@@L16
	mov	dl,1Ah
	jmp	@@L161
@@L16:
	fnstcw	ds:fpudata1
	or	byte ptr ds:fpudata1[1],80h
	fldcw	ds:fpudata1
	fstcw	ds:fpudata1
	wait
	test	byte ptr ds:fpudata1[1],80h	
	jz	@@L162
	mov	dl,1Ch
	jmp	@@L161
@@L162:
	cmp	bl,7
	jb	@@L161
	fninit
	fldpi
	f2xm1
	fld1
	fchs
	fldpi
	fscale
	fstp	st(1)
	fcompp
	fstsw	ax
	wait
	sahf
	jnz	@@L161
	mov	dl,1Eh
@@L161:
	cmp	bl,0Eh
	jb	@@17
	and	dl,3
	or	dl,10h
	jmp	@@30
@@17:
	cmp	bl,0Bh
	jnz	@@nobuilt
	fninit
	fstcw	ds:fpudata1
	cmp	ds:fpudata1,037Fh
	jz	@@builtin
	dec	bl
	jmp	@@31
@@builtin:
	and	dl,1
	or	dl,10h
	jmp	@@nobuilt
@@nobuilt:
	cmp	bl,06
	jnz	@@30
	cmp	dl,0Ch
	jz	@@XL
	cmp	dl,0Dh
	jnz	@@30
@@XL:
	add	dl,8
@@30:
	cmp	bl,0Ah
	jnz	@@31
	cmp	dl,0Ch
	jz	@@487sx
	cmp	dl,0Dh
	jnz	@@31
@@487sx:
	inc	bl
	mov	dl,10h
@@31:
	cmp	bl,18h
	jnz	@@32
	cmp	dl,0Ch
	jz	@@nx587
	cmp	dl,0Dh
	jnz	@@32
@@nx587:
	and	dl,1
	add	dl,20h
@@32:
	cmp	dl,4
	jb	@@L18
	fldenv	ds:fpudata3
@@L18:
ifdef	__DPMI__
	push	ax bx
	mov	ax,dpmiFreeLDTDesc
	mov	bx,ds
	int	31h
	pop	bx ax
endif
	pop	ds
	ASSUME	DS:@data
	ret
	endp
fpudata1	dw	?
fpudata2	dd	?
fpudata3	db	14 dup(?)
tera		dt	?
_53bit_prec	dw	027Fh
denormal	dt	1
op1		db	0F0h,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,03Fh

checkEmu	proc
	.286p
	smsw	ax
	test	al,04
	mov	al,1
	jnz	@@1
	clr	al
@@1:
	ret
	endp

;getQEMMAPI	proc	near
;; get QEMM API entry point
;; returns CF=1 if QEMM found and API entry point got OK.
;	ldx	QEMMId
;	mov	ax,3D00h
;	clr	cx
;	int	21h
;	jc	@@Q
;	mov	bx,ax
;	mov	ax,4402h
;	mov	cx,4
;	ldx	QEMMEntry
;	int	21h
;	mov	ah,3Eh
;	int	21h
;	clc
;@@Q:
;	ret
;	endp
;
;checkQEMM	proc	near
;; get and check QEMM version. Only QEMM v7.04 and up enables WRMSR from V86.
;; returns CF=1 if QEMM found and version is OK.
;	push	bx
;	call	getQEMMAPI
;	jc	@@tooearly
;	mov	ah,3
;	call	dword ptr ds:QEMMEntry
;	cmp	bx,0704h
;	pop	bx
;	jb	@@tooearly
;	stc
;	ret
;@@tooearly:
;	clc
;	ret
;	endp
;
;	EXTRN	QEMMEntry : DWORD
;	EXTRN	QEMMId	  : BYTE

	END
