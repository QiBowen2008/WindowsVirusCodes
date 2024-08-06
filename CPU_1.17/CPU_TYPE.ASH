; ----------------------------------------------------------------------------
; CPU_TYPE.ASH  Too-Much-In-One-So-Don't-Get-Lost(tm) 
;		CPU/FPU feature detection library		Version 1.17
;
; Copyright(c) 1992-95 by B-coolWare.    Written by Bobby Z.
; Portions copyright(c) 1990 by 2B Programmers
; Portions copyright(c) 1994,95 by Lautenaus
; ----------------------------------------------------------------------------
; Uses:
;	UNIDEF.INC	universal macros and defines
;	LSTRING.ASH	Lstring macro
;	DOSINOUT.ASH	WriteStr routine
;	SPEED.ASH	CPU MHz computation routine

ifndef	__INC__
	INCLUDE	UNIDEF.INC
endif
ifndef	__WriteStr__
	__WriteStr = 1
	INCLUDE	DOSINOUT.ASH
endif
ifndef	__Lstring__
	INCLUDE	LSTRING.ASH
endif

	INCLUDE	SPEED.ASH

	NOMASM51

EF_AC		equ	00040000h	; AC bit in EFLAGS register
EF_ID		equ	00200000h	; ID bit in EFLAGS register
MSW_ET		equ	00000010h	; ET bit in MSW register
MSW_NE		equ	00000020h	; NE bit in MSW register

; following constants are returned in AX by CPU_Type routine

cpu8088		equ	0000h	; 8088
cpu8086		equ	0001h	; 8086
cpuNECV20	equ	0002h	; NEC V20
cpuNECV30	equ	0003h	; NEC V30
cpu80188	equ	0004h	; 188
cpu80186	equ	0005h	; 186
cpu80286	equ	0006h	; 286
cpu286		equ	06h	; 286
cpu386		equ	07h	; 386
cpu386sx	equ	07h	; 386sx
cpu80386sxv	equ	0107h	; 386sx in V86 mode
cpu80386sxr	equ	0007h	; 386sx in real mode
cpu386dx	equ	08h	; 386dx
cpu80386dxv	equ	0108h	; 386dx in V86 mode
cpu80386dxr	equ	0008h	; 386dx in real mode
cpu80386slv	equ	0109h	; 386sl in V86 mode
cpu80386slr	equ	0009h	; 386sl in real mode
cpu486sx	equ	0Ah
cpui486sxv	equ	010Ah	; 486sx in V86 mode
cpui486sxr	equ	000Ah	; 486sx in real mode
cpu486dx	equ	0Bh	; 486dx
cpui486dxv	equ	010Bh	; 486dx in V86 mode
cpui486dxr	equ	000Bh	; 486dx in real mode
cpuCx486	equ	0Ch	; Cyrix 486
cpu486slcv	equ	010Ch	; Cyrix 486sx/slc in V86
cpu486slcr	equ	000Ch	; Cyrix 486sx/slc in real mode
cpu486dlc	equ	0Dh
cpu486dlcv	equ	010Dh	; Cyrix 486dx/dlc in V86
cpu486dlcr	equ	000Dh	; Cyrix 486dx/dlc in real mode
cpuPentium	equ	0Eh
cpuPentiumr	equ	000Eh	; Pentium in real mode
cpuPentiumv	equ	010Eh	; Pentium in V86 mode
cpuCxM1		equ	0Fh
cpuCxM1r	equ	000Fh	; Cyrix M1 (586) in real mode
cpuCxM1v	equ	010Fh	; Cyrix M1 (586) in V86 mode
cpuP24T		equ	10h
cpuP24Tr	equ	0010h	; Intel P24T (Pentium OverDrive)
cpuP24Tv	equ	0110h	; Intel P24T in V86 mode
ibm3slc		equ	11h
ibm4slc		equ	12h
ibm4slc2	equ	13h
cpuIBM386SLCr	equ	0011h	; IBM 386SLC
cpuIBM386SLCv	equ	0111h
cpuIBM486SLCr	equ	0012h	; IBM 486SLC
cpuIBM486SLCv	equ	0112h
cpuIBM486SLCr2	equ	0013h	; IBM 486SLC2
cpuIBM486SLCv2	equ	0113h
cpuUMCU5	equ	14h
cpuUMCU5Sr	equ	0014h	; U5-S
cpuUMCU5Sv	equ	0114h
cpuUMCU5Dr	equ	0015h	; U5-D
cpuUMCU5Dv	equ	0115h
cpuam386sx	equ	16h
cpuam386dx	equ	17h
cpuNx586r	equ	0018h	; Nx586
cpuNx586v	equ	0118h
cpuNx586	equ	18h
ibm4bl3		equ	19h
cpuIBM486BL3r	equ	0019h	; IBM 486BL3 Blue Lightning
cpuIBM486BL3v	equ	0119h
cpuAm486	equ	1Ah	; AMD Am486xxx
cpuP54		equ	1Bh
cpuP6		equ	1Ch
cpuP6r		equ	001Ch	; iP6
cpuP6v		equ	011Ch

cpuP7		equ	001Dh	; add new CPUs here

	LOCALS	@@

IF ??version LT 400h	; if current TASM version is earlier than 4.0 then
			; we need macros for P5 instructions

cpuid	equ	<db	0Fh,0A2h>	; Pentium instruction

ENDIF
	.8086

CPU_Type	proc
LOCAL	hasFPUonChip : BYTE

; entry - none
; returns AL = CPU code (see constants above)
;         AH = 1 if in V86, 0 otherwise
;         DL = FPU code (see constants below)

	mov	hasFPUonChip,0
	push	bx cx si
	clr	bx
	push	sp		; this code uses bug in chips prior to
	pop	ax		; 80286: when push sp performed, value
	cmp	ax,sp		; of sp is first decremented and then
				; placed onto the stack. 286 and up
				; handle this instruction correctly, 
				; saving value which sp have upon issue
				; of this command, not after.
	jnz	@@000		; if not equal that it is <286
	mov	ax,7000h
	pushf
	push	ax
	popf
	pushf
	pop	ax
	popf
	mov	bl,6
	and	ah,70h		; check for flags - only 386+ has them
	jz	@@200		; if ah=0 than this is 286
	inc	bx

; distinguish between 386SX/Cx486SX/SLC and 386DX/Cx486DX/DLC chips

; This one uses the fact, that SX/SLC chips has 16-bit data bus and thus
; uses only 16-bit bus interchange format to communicate with coprocessor.
; Due to this limitation the ET bit in MSW is always zero and can't be
; changed, but on DX/DLC it can be flipped.


	.386p
	clr	si
	mov	eax,cr0
	mov	ecx,eax
	xor	eax,MSW_ET	; flipping ET bit
	mov	cr0,eax
	mov	eax,cr0
	mov	cr0,ecx		; restoring previous value of CR0
	xor	eax,ecx		; did it flip ok?
	jz	@@L100
	inc	si		; DX/DLC
@@L100:

;This code that distinguishes a 386 from a 486 depends on
;the 386's inability to toggle the AC bit in the EFLAGS register,
;but the 486 can. This technique is apparently blessed by Intel.

	;Distinguish between 386 and 486
	;Placed into public domain by Compaq Computers.

	.386
	
	mov	ax,sp
	and	sp,0FFFCh	;round down to a dword boundary
	pushfd
	pushfd
	pop	edx
	mov	ecx,edx
	xor	edx,EF_AC	;toggle AC bit
	and	ecx,EF_AC
	push	edx
	popfd
	pushfd
	pop	edx
	popfd			;restore original flags
	mov	sp,ax		;restore original stack pointer
	and	edx,EF_AC
	
	cmp	edx,ecx
	jnz	@@486		;it's a 486
	is0	si,@@386dx	; SX/SLC?
	inc	bx		; 386DX
	jmp	@@386dx

@@486:
	; distinguish between Cyrix 486 and Intel 486+
	mov	bx,cpui486sxr
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
	inc	bx		; Cyrix 486SLC
	inc	bx
	is0	si,@@586
	inc	bx		; Cyrix 486DLC,486SX or 486DX
@@586:

; Check for Pentium or later by attempting to toggle the Id bit in EFLAGS reg:
; if we can't, it's an i486.

	; Pentium detection routine
	; Placed in public domain by Dr. Dobbs Journal

        pushfd			; get current flags
	pop	eax
	mov	ecx,eax
	xor	eax,EF_ID	; attempt to toggle ID bit
	push	eax
	popfd
	pushfd			; get new EFLAGS
	pop	eax
	push	ecx		; restore original flags
	popfd
	and	eax,EF_ID	; if we couldn't toggle ID,
	and	ecx,EF_ID	; then this is i486
	cmp	eax,ecx
	jz	@@486sdx	; do not alter BX
; It's Pentium or later. Use CPUID to get processor family.
	clr	eax		; get processor info
	inc	al
	push	bx		; cpuid destroys bx and dx registers!
IF ??version GE 400h
	.586
ENDIF
	cpuid
	pop	bx
	and	dl,1
	mov	hasFPUonChip,dl
	and	ah,0Fh		; 5 means Pentium
	cmp	ah,4
	jb	@@386dx		; CPUID also works on later models of 486s
	je	@@486sdx2	; Though we shouldn't come here on
				; a 386 it's better to make an extra check for
				; the case we occasionally did.
	push	bx
	push	ax
	clr	eax
	cpuid
	pop	ax
	cmp	ebx,'iryC'	; M1 has Vendor Id = 'CyrixInstead'
	pop	bx
	jnz	@@P5
	cmp	edx,'snIx'
	jnz	@@P5
	cmp	ecx,'daet'
	jnz	@@P5
	mov	bl,cpuCxM1
	jmp	@@L1
	
@@P5:
	mov	bl,ah
	cmp	bl,6
	jz	@@P6
	and	ax,10F0h
	cmp	ah,10h		; Pentium OverDrive
	jz	@@P24T
	cmp	al,20h
	mov	bl,cpuP54	; P54
	jz	@@L1
	mov	bl,cpuPentium	; P5
	jmp	@@L1
@@P24T:
	mov	bl,cpuP24T
	jmp	@@L1
@@P6:
	mov	bl,cpuP6
	jmp	@@L1
@@386dx:
	call	checkIBMmsrs
	jmp	@@L1
@@486sdx2:			; we got here if cpuid works
	clr	eax
	push	bx
IF ??version GE 400h
	.586
ENDIF
	cpuid			; UMC U5 chips support CPUID instruction
	cmp	ecx,' CMU'	; returning 'UMC UMC UMC ' as Vendor Id.
	jnz	@@notUMC
	pop	bx
	clr	eax
	inc	al
	cpuid
	and	al,0F0h
	cmp	al,10h
	mov	bl,cpuUMCU5	; U5-S
	jnz	@@L1
	inc	bl		; U5-D
	jmp	@@L1
@@notUMC:
	cmp	ebx,'htuA'	; AMD Am486DXx+ has Vendor Id = 'AuthenticAMD'
	pop	bx
	jnz	@@notAMD
	cmp	edx,'itne'
	jnz	@@notAMD
	cmp	ecx,'DMAc'
	jnz	@@notAMD
	mov	bl,cpuAm486
	push	bx
	clr	eax
	inc	al
	cpuid
	pop	bx
	and	al,0F0h
	or	bh,al
	jmp	@@L1
@@notAMD:
	call	checkIBMmsrs3	; check if IBM MSRs present (will only work in
	jc	@@L1		; real mode)
	mov	bl,cpu486sx
	clr	eax		; get processor info
	inc	al
	push	bx		; cpuid destroys bx and dx registers!
IF ??version GE 400h
	.586
ENDIF
	cpuid
	pop	bx
	isn0	al,@@setNibble	; 0 and 1 both means DX
	inc	al
@@setNibble:
	or	bh,al		; put model code into bh's high nibble
	jmp	@@L1
@@486sdx:

	; distinguish between i486dx and i486sx processors
	; based on 486sx's inability to toggle NE bit of MSW

	.486p
	call	isInOSZwei	; OS/2 won't allow to flip NE bit anyway
	jnz	@@L1
	cmp	bl,cpu486dlc
	jae	@@L1		; avoid test on Cyrix 486s
	mov	eax,cr0
	mov	ecx,eax
	db	66h,83h,0E0h,0DFh
	and	eax,0FFFFFFDFh	; flip off NE bit of MSW
	mov	cr0,eax
	mov	eax,cr0
	cmp	eax,ecx
	jnz	@@486dx
	or	eax,MSW_NE	; flip on NE bit of MSW
	mov	cr0,eax
	mov	eax,cr0
	cmp	eax,ecx
	jnz	@@486dx
	dec	bx
@@486dx:
	inc	bx
	mov	eax,ecx
	mov	cr0,eax
	call	checkIBMmsrs3

@@L1:
; detect a NexGen Nx586 CPU
	cmp	bl,07h		; should test as 386 only
	jb	@@L101
	cmp	bl,08h
	ja	@@L101
	mov	ax,5555h
	xor	dx,dx
	mov	cx,2
	div	cx
	jnz	@@L101		; Nx586 doesn't change ZF on division while
	mov	bl,cpuNx586	; other 386s do.
@@L101:
	.286p
	smsw	ax
	and	al,1
	or	bh,al		; get the VM flag into bh
	jmp	@@200
@@000:
	mov	bl,4		; assume this is 186/188
	mov	cl,33
	clr	ax
	dec	ax
	shl	ax,cl
	jnz	@@100		; 186/188 does not actually shift
				; more that 32 bits. It shifts only
				; n mod 32 bits, where n is number of
				; bits to shift.
	mov	bl,2		; assume NEC family
	clr	cx
	dec	cx
;	rep  es: lodsb  - incorrect order of prefixes and thus would
;			  not work as repeated command on Intel's CPUs.
;			  8086/88 accepts only es: rep lodsb order. But
;			  NEC Vxx CPUs process multiple prefix repeated
;			  instructions correctly regardless of their
;			  order.

	db	0F3h,26h,0ACh
;		rep  es: lodsb

	jcxz	@@100		; was repeated cx times -> NEC V20/V30
	clr	bx		; good old 88/86
@@100:
	clr	ax		; clear ax, so that test for new CPU won't work
	call	Test_Buffer	; later...
	jcxz	@@200		; prefetch buffer length < 6 bytes -> 88 / V20
	inc	bx		; prefetch buffer length = 6 bytes -> 86 / V30
@@200:
	mov	dl,10h
	cmp	hasFPUonChip,0
	jnz	@@noCheck
@@checkFPU:
	call	FPU_Type
@@noCheck:
	call	checkWeitek
@@Q:
	mov	ax,bx
	pop	si cx bx
	.8086
	ret
	endp

isInOSZwei	proc near
	push	ax bx
	mov	ax,4010h
	int	2Fh
	cmp	ax,4010h
	pop	bx ax
	ret
	endp


Test_Buffer	proc near	; (C) 2B Programmers
	push	es di
	mov	_bpcs[@@0],41h	; to make this routine reentrant
	std
	push	cs
	pop	es
	ldi	@@2
	mov	al,_bpcs[@@1]
	mov	cx,3
	cli
	rep	stosb
	cld		; 1
	nop		; 2
	nop		; 3
	nop		; 4	<- 80x88 will cut here and inc cx instruction
@@0:	inc	cx	; 5	   will be overwritten by sti, else we'll get
@@1:			;	   cx = 1, which indicates 80x86
	sti		; 6
@@2:	
	sti
	pop	di es
	ret
	endp

	db	13,10
	db	'				Too much is not enough...',13,10
	db	'					(Deep Purple)',13,10
	db	13,10
	db	'TMi0SDGL(tm) CPU/FPU feature detection library  Version 1.17',13,10
	db	'Copyright(c) 1992-95 by B-coolWare.     Released as freeware.',13,10


checkWeitek	proc near
	cmp	bl,7
	jb	@@1
	.386
	clr	eax
	int	11h
	test	eax,1000000h
	.8086
	jz	@@1
	or	dl,1		; Weitek 1167 present
@@1:
	ret
	endp

checkEmu	proc
; returns CF = 1 if FPU emulator detected, CF = 0 otherwise
	.286p
	push	ax
	smsw	ax
	test	al,04
	stc
	jnz	@@1
	clc
@@1:
	pop	ax
	ret
	endp

	.8086

FPU_Type	proc near
;
; on entry : BL = CPU code	!! Required !!
;
	.286p
	.287
	mov	dl,2		; assume no coprocessor present
	fninit
	clr	cx
	jmp	$+2		; just to make sure we have enough time for
				; FPU to initialize
	mov	fpubuf_w,5A5Ah
	fnstsw	fpubuf_w
	mov	ax,fpubuf_w
	isn0	al,@@L161	; FPU wasn't initialized - no FPU at all
	fnstcw	fpubuf_w	; check the control word also
	mov	ax,fpubuf_w
	and	ax,103Fh
	cmp	ax,3Fh
	jne	@@L161
	mov	dl,4		; assume 8087
	fstenv	fpuenv
	and	fpubuf_w,0FF7Fh
	fldcw	fpubuf_w
	fdisi
	fstcw	fpubuf_w
	wait
	test	fpubuf_w,80h
	jnz	@@L161
	mov	dl,8		; assume 80287
	fninit			; checking if -Inf <> +Inf
	fld1			; 287 erroneously claim that they are equal
	fldz
	fdivp	st(1),st
	fld	st
	fchs
	fcompp
	fstsw	fpubuf_w
	wait
	mov	ax,fpubuf_w
	sahf
	jz	@@L161		; -Inf <> +Inf -> 287XL or 387 and up
	mov	dl,0Ch		; assume 80387
	cmp	bl,cpu286	; IIT x87's cannot work with CPUs prior to 286
	jb	@@L35		; so we disable the test on them too.
@@checkIIT:
	fld	ds:[denormal]
	fadd	st(0),st	; IIT will produce zero result while all others
	fnstsw	ax		; won't
	test	al,02h
	jnz	@@L35		; not an IIT chip
	cmp	dl,0Ch		; tested as 80387?
	jz	@@300
	mov	dl,22		; this is IIT 2C87
	jmp	@@L161
@@300:
	cmp	bl,0Ah		; it's a 486?
	jb	@@301
	mov	dl,22h		; assume IIT 4C87
	jmp	@@L161
@@301:
	mov	dl,24		; this is IIT 3C87
	jmp	@@L161
@@L35:
	fninit
	fldpi
	f2xm1
	fstp	fpubuf_d
	wait
	cmp	word ptr [fpubuf_d+2],3FC9h
	jne	@@L15
	or	dl,2		; this is Cyrix ?C87
@@L15:
; testing for ULSI FPUs
	fninit
	fldcw	_53bit_prec
	fld	tbyte ptr op1
	fld1
	faddp	st(1),st
	fstp	tera
	fnstsw	ax
	wait
	test	al,20h
	jnz	@@L16
	cmp	byte ptr [tera],0F8h
	jnz	@@L16
	cmp	byte ptr [tera+9],40h
	jnz	@@L16
	mov	dl,1Ah
	jmp	@@L161
@@L16:
; testing for Cyrix EMC87
	fnstcw	fpubuf_w
	wait
	or	byte ptr fpubuf_w[1],80h
	fldcw	fpubuf_w
	fstcw	fpubuf_w
	wait
	test	byte ptr fpubuf_w[1],80h
	jz	@@L162
	mov	dl,1Ch
	jmp	@@L161
@@L162:
	cmp	bl,cpu386
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
	cmp	bl,cpu486dx	; 486dx?
	jne	@@nobuilt
	mov	fpubuf_w,0
	fninit			; test if this is actually an i486sx
	fstcw	fpubuf_w
	cmp	fpubuf_w,37Fh
	jz	@@builtin
	dec	bl		; this is 486sx - some tricky sx'es pass thru
				; sx-specific test
	jmp	@@31
@@builtin:
	and	dl,1
	or	dl,10h		; built-in coprocessor
	jmp	@@nobuilt
@@nobuilt:
	cmp	bl,cpu286	; 286...?
	jnz	@@30
	cmp	dl,0Ch		; ...and FPU tested as 387...?
	jz	@@21
	cmp	dl,0Dh
	jnz	@@30
@@21:
	add	dl,8		; then assume 80287XL - tricky
@@30:
	cmp	bl,cpu486sx	; i486sx ?
	jnz	@@31
	cmp	dl,0Ch		; 387?
	jz	@@487sx
	cmp	dl,0Dh
	jnz	@@31
@@487sx:
	inc	bl		; assume 486DX or 487SX
	mov	dl,10h		; assume internal FPU
@@31:
	cmp	bl,cpuNx586	; Nx586?
	jnz	@@32
	cmp	dl,0Ch		; there's an 386-compatible FPU?
	jz	@@nx587
	cmp	dl,0Dh
	jnz	@@32
@@nx587:
	and	dl,1		; assume Nx587
	add	dl,20h
@@32:
	cmp	dl,4		; any 87 present?
	jb	@@L18
	fldenv	fpuenv	; yes - restore 87 environment
@@L18:
	ret
	endp
fpubuf_w	dw	?
fpubuf_d	dd	?
fpuenv		db	14 dup(?)
tera		dt	?
_53bit_prec	dw	02F7h
denormal	dt	1
op1		db	0F0h,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,03Fh


ifdef	__PRINT_CPU__

Lstring stepIs,<  Cyrix CPU Step: >
Lstring revIs,<, Revision: >

CPUs	dw	offset I88
	dw	offset I86
	dw	offset V20
	dw	offset V30
	dw	offset I188
	dw	offset I186
	dw	offset I286
	dw	offset I386SX
	dw	offset I386DX
	dw	offset I386SL
	dw	offset I486SX
	dw	offset I486DX
	dw	offset Cyr486
	dw	offset Cyr486
	dw	offset I586
	dw	offset CxM1
	dw	offset P24T
	dw	offset ibm386slc
	dw	offset ibm486slc
	dw	offset ibm486slc2
	dw	offset U5S
	dw	offset U5D
	dw	offset am386sx
	dw	offset am386dx
	dw	offset Nx586
	dw	offset ibm486bl3
	dw	offset am486dx
	dw	offset p54
	dw	offset P6

Lstring	I88,<Intel 8088>
Lstring	I86,<Intel 8086>
Lstring	V20,<NEC V20>
Lstring	V30,<NEC V30>
Lstring	I188,<Intel 80188>
Lstring	I186,<Intel 80186>
Lstring I286,<Intel 80286>
Lstring I386SX,<Intel 80386SX>
Lstring I386DX,<Intel 80386DX>
Lstring I386SL,<IBM 386SL>
Lstring I486SX,<Intel i486SX>
Lstring I486DX,<Intel i486DX or i487SX>
Lstring Cyr486,<Cyrix Cx486>
Lstring I586,<Intel Pentium>
Lstring CxM1,<Cyrix M1 (586)>
Lstring i486,<Intel i486>
Lstring P24T,<Intel iP24T (Pentium OverDrive)>
Lstring am386sx,<AMD Am386SX>
Lstring am386dx,<AMD Am386DX>
Lstring ibm386slc,<IBM 386SLC>
Lstring ibm486slc,<IBM 486SLC>
Lstring ibm486slc2,<IBM 486SLC2>
Lstring U5S,<UMC U5-S>
Lstring U5D,<UMC U5-D>
Lstring Nx586,<NexGen Nx586>
Lstring ibm486bl3,<IBM 486BL3 (Blue Lightning)>
Lstring p54,<Intel iP54>
Lstring am486dx,<AMD Am486DX>
Lstring Ti486SXL,<Texas Instruments Ti486SXL>
Lstring P6,<Intel P6>

ModelTable	label	word
	dw	offset strDX
	dw	offset strDX
	dw	offset strSX
	dw	offset strDX2
	dw	offset strSL
	dw	offset strSX2
	dw	offset strBlank
	dw	offset strDX2WB
	dw	offset strDX4
	dw	offset strBlank
	dw	offset strBlank
	dw	offset strBlank
	dw	offset strBlank
	dw	offset strBlank
	dw	offset strBlank
AmModels	label	word
	dw	offset strBlank
	dw	offset strBlank
	dw	offset strBlank
	dw	offset str2
	dw	offset strBlank
	dw	offset strBlank
	dw	offset strBlank
	dw	offset str2Plus
	dw	offset str4
	dw	offset str4Plus
	dw	offset strBlank
	dw	offset strBlank
	dw	offset strBlank
	dw	offset strBlank
	dw	offset strBlank
	dw	offset strBlank

CxModels	label	byte
	db	0
	dw	offset Cx0
	db	1
	dw	offset Cx1
	db	2
	dw	offset Cx2
	db	3
	dw	offset Cx3
	db	4
	dw	offset Cx4
	db	5
	dw	offset Cx5
	db	6
	dw	offset Cx5
	db	7
	dw	offset Cx7
	db	10h
	dw	offset Cx10
	db	11h
	dw	offset Cx11
	db	12h
	dw	offset Cx12
	db	13h
	dw	offset Cx13
	db	1Ah
	dw	offset strDX
	db	1Bh
	dw	offset Cx1B
	db	0FFh

Lstring	Cx0,<SLC>
Lstring Cx1,<DLC>
Lstring Cx2,<SL2>
Lstring Cx3,<DL2>
Lstring Cx4,<SR>
Lstring Cx5,<DR>
Lstring Cx6,<SR2>
Lstring Cx7,<DR2>
Lstring Cx10,<S>
Lstring Cx11,<S2>
Lstring Cx12,<SE>
Lstring Cx13,<S2E>
Lstring Cx1B,<DX2>

Lstring strDX,<DX>
Lstring strSX,<SX>
Lstring strDX2,<DX2/OverDrive>
Lstring strSL,<SL>
Lstring strSX2,<SX2>
Lstring strDX4,<DX4>
Lstring strDX2WB,<DX2WB (P24D)>
Lstring strBlank,<>
Lstring str2,<2>
Lstring str2Plus,<2+>
Lstring str4,<4>
Lstring str4Plus,<4+>

FPUType	dw	?

ifdef	__PRINT_SPEED__
Lstring MHz,<MHz>
Lstring Comma,<, >
endif

IFDEF	__PRINT_STEP__
checkWindoze	proc
	push	ax dx
	mov	ax,1600h
	int	2Fh
	is0	al,@@nowin
	cmp	al,80h
	jz	@@nowin
	stc
	jmp	@@Qw
@@nowin:
	clc
@@Qw:
	pop	dx ax
	ret
	endp
ENDIF

ifdef	__PRINT_SPEED__

	.8086
WriteInt	proc		; it's quite unoptimal, but it works...
ARG	anInt	: Byte
LOCAL	aStr	: BYTE : 5
	lea	di,aStr
	push	ss
	pop	es
	inc	di
	clr	cl
	mov	al,anInt
	clr	ah
	mov	bl,100
	cmp	al,bl
	jb	@@1
	div	bl
	add	al,30h
	stosb
	inc	cl
@@1:
	mov	al,anInt
	clr	ah
	mov	bl,10
	cmp	al,bl
	jb	@@2
	div	bl
	cmp	al,10
	jb	@@10
	sub	al,al
@@10:
	add	al,30h
	stosb
	inc	cl
@@2:
	mov	al,anInt
	clr	ah
	mov	bl,10
	div	bl
	xchg	ah,al
	add	al,30h
	stosb
	inc	cl
	lea	di,aStr
	mov	al,cl
	stosb
	push	ds
	push	ss
	pop	ds
	lea	dx,aStr
	call	WriteStr
	pop	ds
	ret
	endp

endif	; __PRINT_SPEED__

	.8086	
print_CPU	proc
LOCAL	cpuSpeed : BYTE, Step : BYTE, Rev : BYTE, isCx : Byte
	mov	isCx,0
	call	CPU_Type	; get CPU and FPU codes
	clr	dh
	mov	FPUType,dx	; save FPU code in variable
	push	ax
	call	intCPUSpeed	; get processor clock speed in MHz
	mov	cpuSpeed,al
	pop	ax
	cmp	al,cpuCx486
	jb	__1
	cmp	al,cpu486dlc
	ja	__1
	push	ax
	call	getCyrixModel	; determine Cyrix 486 model
	mov	Step,ah
	and	ah,0Fh
	add	ah,30h
	mov	Rev,ah
	shr	Step,4
	add	Step,30h
	mov	isCx,1
	lsi	CxModels
__2:
	cmp	al,byte ptr [si]
	jz	__3
	cmp	byte ptr [si],0FFh
	jz	@@new486
	add	si,3
	jmp	__2
__3:
	cmp	al,1Ah
	jb	@@121
	cmp	al,1Bh
	ja	@@121
	and	FPUType,1
	add	FPUType,10h
@@121:
	cmp	al,0FEh
	jz	@@ti
	mov	dx,word ptr [si+1]
	push	dx
	ldx	Cyr486
	call	WriteStr
	pop	dx
	jmp	@@new486
@@ti:
	ldx	Ti486SXL
	jmp	@@new486
__1:
	push	ax
	cmp	al,cpuAm486	; Am486 detected?
	jnz	@@intel
	and	ah,0F0h
	mov	cl,4
	shr	ah,cl
	mov	bl,ah
	clr	bh
	shl	bx,1
	mov	dx,AmModels[bx]
	push	dx
	ldx	am486dx
	call	WriteStr
	pop	dx
	jmp	@@new486
@@intel:
	cmp	al,cpu486sx	; 486 detected?
	jnz	@@older
	and	ah,0F0h		; and it supports CPUID?
	jz	@@older
	mov	cl,4		; determine CPU model
	shr	ah,cl
	mov	bl,ah
	clr	bh
	shl	bx,1
	mov	dx,ModelTable[bx]
	push	dx
	ldx	i486
	call	WriteStr
	pop	dx
	jmp	@@new486
@@amsx:
	cmp	cpuSpeed,35	; freq. > 33 means AMD 386SX
	jb	@@printIt
	mov	bl,cpuam386sx
	jmp	@@printIt
@@amdx:
	cmp	cpuSpeed,35	; freq. > 33 means AMD 386DX
	jb	@@printIt
	mov	bl,cpuam386dx
	jmp	@@printIt
@@older:
	mov	bl,al
	cmp	bl,cpu386sx
	je	@@amsx
	cmp	bl,cpu386dx
	je	@@amdx
@@printIt:
	clr	bh
	shl	bx,1		; now get string offset from table
	mov	dx,CPUs[bx]
@@new486:
	call	WriteStr
	pop	ax

ifdef	__PRINT_SPEED__
	push	ax
	ldx	Comma
	call	WriteStr
	mov	al,cpuSpeed
	push	ax
	call	WriteInt
	ldx	MHz
	call	WriteStr
	pop	ax
endif

ifdef	__PRINT_STEP__
	call	checkWindoze
	jc	@@Exit
	cmp	al,cpu386dx	; 386dx ?
	jnz	@@Exit
	call	print_Step
else
	cmp	al,cpu386
	jb	@@noSMM
	cmp	al,0Eh		; P5 ?
	jb	@@checkSMM
	cmp	al,10h
	jbe	@@noSMM		; P5, M1 and P24T don't need SMM detection
	cmp	al,1Bh
	jae	@@noSMM		; so does P54, P6, etc.
@@checkSMM:
	call	isSMMable	; this routine can crash DOS session under
				; Windows 95 if SMM is not supported by CPU
	test	ax,ax
	jz	@@noSMM
	ldx	SLEnhanced
	call	WriteStr
@@noSMM:
endif	; __PRINT_STEP__
@@Exit:
	ldx	EndLine
	call	WriteStr
	cmp	isCx,1
	jnz	@@noCyrix
	ldx	stepIs
	call	WriteStr
	mov	dl,Step
	mov	ah,2
	int	21h
	ldx	revIs
	call	WriteStr
	mov	dl,Rev
	mov	ah,2
	int	21h
	ldx	EndLine
	call	WriteStr
@@noCyrix:
ifdef	__PRINT_FPU__
	call	print_FPU
	ldx	EndLine
	call	WriteStr
endif
	ret
	endp

endif	; __PRINT_CPU__

ifdef	__PRINT_STEP__
__386STEP__	EQU	1
endif

ifdef	__386STEP__

stepB0	EQU	0
stepB1	EQU	1
stepD0	EQU	2

ifdef	__PRINT_STEP__

stepTable	label	word
	dw	offset B0Str
	dw	offset B1Str
	dw	offset D0Str

Lstring	B0Str,<B0 or earlier>
Lstring B1Str,<B1>
Lstring D0Str,<D0 or later>
Lstring stepStr,<, chip step is >

print_Step	proc
	.8086
	ldx	stepStr
	call	WriteStr
	call	i386step
	mov	bl,al
	clr	bh
	shl	bx,1
	mov	dx,stepTable[bx]
	call	WriteStr
	ret
	endp

endif

; this routine determines 386DX chip stepping.
; ripped from Phar Lap TellMe utility.

; returns:
;		ax = 0  ->  chip is step B0 or earlier
;		ax = 1	->  chip is step B1
;		ax = 2  ->  chip is step D0 or later

i386step	proc
	.386
	push	bx cx dx si di es ds
	mov	ax,3506h
	int	21h
	push	cs
	pop	ds
	mov	dx,offset @@int06h
	mov	ax,2506h
	int	21h
	clr	cx
	inc	cx
;	.486
;	cmpxchg	ds:[bx+si],al	; this command sequence cause INT 06
	db	0Fh,0A6h,00h	; to occur on 386s later than step B0.

				; TASM does not generate this code when
				; using mnemonics. Generated code 
				; differs from that specified in db
				; but decodes in the same instruction.
@@B0:
	mov	dx,bx
	push	es
	pop	ds
	mov	ax,2506h
	int	21h
	jcxz	@@checkB1
	clr	ax
	jmp	@@Qstep
@@int06h:
	clr	cx
	add	sp,4
	popf
	jmp	@@B0
@@buf	dw	0
@@checkB1:
	mov	di,offset @@buf
	mov	dx,80h
	inc	cx		; cx = 0 if we got here
	mov	ax,cx
	inc	ax
	cld
	push	ax
	rep	insb		; repeating insb 1 times on B1 step chip does
				; not reset cx to zero.
	pop	ax
	jcxz	@@Qstep
	dec	ax
@@Qstep:
	pop	ds es di si dx cx bx
	ret
	endp

endif

ifdef	__PRINT_FPU__

FPUs	dw	offset fpuUnknown	; ?
	dw	offset fpuUnknown	; ?
	dw	offset fpuNone		; ---
	dw	offset fpuWeitek	; Weitek
	dw	offset fpu87		; 8087
	dw	offset fpu87W		; 8087 + Weitek
	dw	offset fpu487sx		; i487sx
	dw	offset fpu487sxw	; i487sx + Weitek
	dw	offset fpu287		; 80287
	dw	offset fpu287W		; 80287 + Weitek
	dw	offset fpu2C87		; Cyrix 2C87
	dw	offset fpu2C87W		; Cyrix 2C87 + Weitek
	dw	offset fpu387		; 80387
	dw	offset fpu387W		; 80387 + Weitek
	dw	offset fpu3C87		; Cyrix 3C87
	dw	offset fpu3C87W		; Cyrix 3C87 + Weitek
	dw	offset fpuBuiltIn	; 486 internal
	dw	offset fpuBuiltInW	; 486 internal + Weitek
	dw	offset fpu4C87		; Cyrix 4C87
	dw	offset fpu4C87W		; Cyrix 4C87 + Weitek
	dw	offset fpu287XL		; Intel 80287XL
	dw	offset fpu287XLW	; Intel 80287XL + Weitek
	dw	offset fpuIIT2C87	; IIT 2C87
	dw	offset fpuIIT2C87W	; IIT 2C87 + Weitek
	dw	offset fpuIIT3C87	; IIT 3C87
	dw	offset fpuIIT3C87W	; IIT 3C87 + Weitek
	dw	offset fpuULSI83C87	; ULSI 83C87
	dw	offset fpuULSI83C87W	; ULSI 83C87 + Weitek
	dw	offset fpuEMC87		; Intel EMC87
	dw	offset fpuEMC87W	; Intel EMC87 + Weitek
	dw	offset fpuCT		; C&T 38700
	dw	offset fpuCTW		; C&T 38700 + Weitek
	dw	offset Nx587		; NexGen Nx587
	dw	offset Nx587W		; NexGen Nx587 + Weitek
	dw	offset fpuIIT4C87	; IIT 4C87
	dw	offset fpuIIT4C87W	; IIT 4C87 + Weitek

Lstring SLEnhanced,<  [SL Enhanced]>
Lstring FPUis,<Coprocessor: >
Lstring fpuWeitek,<Weitek>
Lstring fpuUnknown,<Unknown>
Lstring fpuNone,<None>
Lstring fpuBuiltIn,<Internal>
Lstring fpu87,<Intel 8087>
Lstring fpu287,<Intel 80287>
Lstring fpu287XL,<Intel 80287XL>
Lstring fpu387,<Intel 80387>
Lstring fpu487sx,<Intel i487sx>
Lstring fpu2C87,<Cyrix 82x87>
Lstring fpu3C87,<Cyrix 83x87>
Lstring fpu4C87,<Cyrix 84x87>
Lstring fpuULSI83C87,<ULSI 83x87>
Lstring fpuEMC87,<Cyrix EMC87>
Lstring fpuCT,<C&T 38700>
Lstring fpuIIT2C87,<IIT 2C87>
Lstring fpuIIT3C87,<IIT 3C87>
Lstring fpuIIT4C87,<IIT 4C87>
Lstring Nx587,<NexGen Nx587>
Lstring fpu487sxw,<Intel i487sx and Weitek>
Lstring fpuBuiltInW,<Internal and Weitek>
Lstring fpu87W,<Intel 8087 and Weitek>
Lstring fpu287W,<Intel 80287 and Weitek>
Lstring fpu287XLW,<Intel 80287XL and Weitek>
Lstring fpuIIT2C87W,<IIT 2C87 and Weitek>
Lstring fpuIIT3C87W,<IIT 3C87 and Weitek>
Lstring fpuIIT4C87W,<IIT 4C87 and Weitek>
Lstring fpu387W,<Intel 80387 and Weitek>
Lstring fpu2C87W,<Cyrix 2C87 and Weitek>
Lstring fpu3C87W,<Cyrix 3C87 and Weitek>
Lstring fpu4C87W,<Cyrix 4C87 and Weitek>
Lstring fpuULSI83C87W,<ULSI 83x87 and Weitek>
Lstring fpuEMC87W,<Cyrix EMC87 and Weitek>
Lstring fpuCTW,<C&T 38700 and Weitek>
Lstring Nx587W,<NexGen Nx587 and Weitek>
Lstring EndLine,<>,CrLf
Lstring Emulator,<, Emulated>

print_FPU	proc
	ldx	FPUis
	call	WriteStr
	mov	bx,FPUType
	shl	bx,1
	mov	dx,FPUs[bx]
	call	WriteStr
	cmp	al,cpu286
	jb	@@1p
	call	checkEmu
	jnc	@@1p
	ldx	Emulator
	call	WriteStr
@@1p:
	ret
	endp
endif

	.386p

IF ??version LT 400h

rdmsr	macro	Reg			; new 486 and Pentium instruction
	mov	ecx,Reg			; read MSR #ECX into EDX:EAX
	db	0Fh,32h
	endm
wrmsr	macro	Reg			; new 486 and Pentium instruction
	mov	ecx,Reg			; write MSR #ECX from EDX:EAX
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
;	call	checkQEMM	; ...or under QEMM 7.04 and up. ???
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
	rdmsr	1000h
ELSE
	mov	ecx,1000h
	.586p
	rdmsr
ENDIF				; We'll get Exception 13 if this MSR is not
	stc			; valid or Exception 06 if this is not IBM386.
	mov	bl,ibm3slc	; IBM 386SLC
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
	mov	ax,350Dh	; we don't need to intercept Exception #6 here,
	int	21h		; for it is proved that RDMSR instruction is
	push	es		; valid for current CPU. But the register we're
	push	bx		; trying to read may be invalid...
	push	cs
	pop	ds
	mov	dx,offset @@trap0D2
	mov	ax,250Dh
	int	21h
IF ??version LT 400h
	rdmsr	1002h		; try to read 486SLC2 specific MSR
ELSE
	mov	ecx,1002h
	.586p
	rdmsr
ENDIF
	mov	bl,ibm4slc2	; IBM 486SLC2
	jmp	@@Qi2
@@trap0D2:
	add	sp,4
	popf
	stc
	mov	bl,ibm4slc	; IBM 486SLC
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
	call	checkIBMmsrs2
	jnc	@@Qi4
	push	bx
	push	ds
	mov	ax,350Dh	; we don't need to intercept Exception #6 here,
	int	21h		; for it is proved that RDMSR instruction is
	push	es		; valid for current CPU. But the register we're
	push	bx		; trying to read may be invalid...
	push	cs
	pop	ds
	mov	dx,offset @@trap0D3
	mov	ax,250Dh
	int	21h
IF ??version LT 400h
	rdmsr	1004h		; try to read 486BL3 specific MSR
ELSE
	mov	ecx,1004h
	.586p
	rdmsr
ENDIF
	mov	bl,ibm4bl3
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


;getQEMMAPI	proc	near
;; get QEMM API entry point
;; returns CF=1 if QEMM found and API entry point got OK.
;	ldx	QEMMId
;	mov	ax,3D00h
;	clr	cx
;	int	21h
;	jc	@@Qa
;	mov	bx,ax
;	mov	ax,4402h
;	mov	cx,4
;	ldx	QEMMEntry
;	int	21h
;	mov	ah,3Eh
;	int	21h
;	clc
;@@Qa:
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
;QEMMEntry	dd	?
;QEMMId		db	'QEMM386$',0

	.386

read_chipset	macro
; macro for reading chipset register specified in AL
	out	22h,al
	in	al,23h
	endm

write_chipset	macro
; macro for writing a value in AH to chipset register specified in AL 
	out	22h,al
	mov	al,ah
	out	23h,al
	endm

getCyrixModel	proc
	pushf
	cli
	clr	dx
	mov	al,0C2h
	read_chipset
	mov	cl,al
	or	al,4
	mov	bh,al
	mov	ah,al
	mov	al,0C2h
	write_chipset
	mov	al,0C0h
	read_chipset
	mov	al,0C2h
	read_chipset
	or	al,4
	cmp	al,4
	jne	@@4
	mov	al,cl
	and	al,not 4
	mov	ah,al
	mov	al,0C2h
	write_chipset
	mov	al,0C0h
	read_chipset
	mov	al,0C2h
	read_chipset
	and	al,4
	jnz	@@4
	inc	dh
@@4:
	mov	al,0C2h
	mov	ah,cl
	write_chipset
	mov	al,0C3h
	read_chipset
	mov	cl,al
	or	al,2
	mov	bh,al
	mov	ah,al
	mov	al,0C3h
	write_chipset
	mov	al,0C0h
	read_chipset
	mov	al,0C3h
	read_chipset
	or	al,2
	cmp	al,2
	jne	@@5
	mov	al,cl
	and	al,not 2
	mov	ah,al
	mov	al,0C3h
	write_chipset
	mov	al,0C0h
	read_chipset
	mov	al,0C3h
	read_chipset
	and	al,2
	jnz	@@5
	inc	dl
@@5:
	mov	al,0C3h
	mov	ah,cl
	write_chipset
	test	dl,dl
	jz	@@6
	clr	bx
	clr	cx
	mov	al,0FEh
	read_chipset
	mov	bl,al	; bl = DIR0
	mov	al,0FFh
	read_chipset
	mov	bh,al	; bh = DIR1
	jmp	@@7
@@6:
	clr	bx	; unknown Cyrix chip
@@7:
	sti
	mov	ax,bx
	popf
	ret
	endp

int6H:			; INT 6 (invalid instruction) trap hook
	clr	ax
	add	sp,4	; restore stack and jump past offending instruction
	popf
	jmp	Past_invalid

isSMMAble	proc
	push	ds
	mov	ax,3506h	; get current INT 6 vector
	int	21h
	mov	ax,2506h	; hook INT 6
	mov	dx,offset int6H
	push	cs
	pop	ds
	int	21h
	pop	ds
	clr	ax		; AX = 0
	mov	cl,1		; CL = 1
	mov	dl,ds:[bx+si]	; save byte at [bx+si] in dl

;	The following code ( 0F12C1 ) is SMM instruction UMOV AL,CL that moves
;	data from/to main memory nevertheless the processor is in SMM or not.

;	Cyrix chips doesn't generate INT 6 on this instruction. Instead, two
;	bytes 0F12 gets skipped and the next instruction appears to be
;	{rol word ptr [bx+si],0} ( C10000 ).

	db	0Fh
	adc	al,cl
	add 	ds:[bx+si],al
Past_Invalid:
	mov	ds:[bx+si],dl	; restoring possibly damaged data
	push	ds
	push	es
	pop	ds
	mov	dx,bx
	push	ax
	mov	ax,2506h	; restore INT 6 vector
	int	21h
	pop	ax
	pop	ds
	ret
	endp
