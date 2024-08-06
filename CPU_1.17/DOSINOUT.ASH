;------------------------------------------------------------------------------
; DOSINOUT.ASH DOS Input/Output Routines
;
; TopASM(tm) Assembly Language Library  Version 1.05
; Copyright(c) 1992 by B-coolWare.   Written by Bobby Z.
;------------------------------------------------------------------------------


STDIN	EQU	0
STDOUT	EQU	1

__DOSINOUT__	EQU	1

ifdef	__WriteStr__

ifdef	__check_Quiet__
Quiet	db	0
endif

WriteStr	proc
; DS:DX -> Lstring to print
ifdef	__check_Quiet__
	cmp	Quiet,1
	jz	@@Q
endif
	push	ax bx cx dx si
	cld
	mov	si,dx
	lodsb
ifdef	__use_386__
	movzx	cx,al
else
	mov	cl,al
	sub	ch,ch
endif
	mov	bx,STDOUT
	inc	dx
	mov	ah,40h
	int	21h
	pop	si dx cx bx ax
@@Q:
	ret
	endp
endif

ifdef	__WriteStrFile__

WriteStrFile	proc
; DS:DX -> Lstring to write
; BX    =  file handle to write to

	push	ax cx dx si
	mov	ah,40h
	mov	si,dx
	cld
	lodsb
ifdef	__use_386__
	movzx	cx,al
else
	mov	cl,al
	sub	ch,ch
endif
	inc	dx
	int	21h
	pop	si dx cx ax
	ret
	endp
endif
