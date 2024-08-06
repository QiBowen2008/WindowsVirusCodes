; -----------------------------------------------------------------------------
; CXMODEL.ASM  Cyrix/TI 486 CPU distinguishing code for TMi0SDGL Version 1.17
;
; Copyright(c) 1995 by Lautenaus   Written by Sergei Frolov
; Copyright(c) 1992-95 by B-coolWare., adapted for TMi0SDGL by Bobby Z.
; Included in TMi0SDGL by author's permission.
; -----------------------------------------------------------------------------
; This code makes direct reads and writes to chipset registers, thus it may
; not work properly on some chipsets. It was initially written on NEAT chipset
; and tested on BIOTEQ and HiNT chipsets. Worked fine so far...
;
	INCLUDE	HEADER.ASH

	.CODE

	.386

	PUBLIC	getCyrixModel	; returns Cyrix CPU model indentifier
	PUBLIC  IsCyrix		; returns 1 if Cyrix CPU, 0 otherwise

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

IsCyrix	proc
	mov	bx,100h
	mov	ah,7Fh
	cwd
	sahf
	div	bx
	lahf
	xor	al,ah
	and	al,0D5h
	mov	al,0
	jnz	@@1
	inc	al
@@1:
	ret
	endp


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
	mov	bl,al	; bl = DIR0 (model)
	mov	al,0FFh
	read_chipset
	mov	bh,al	; cl = DIR1 (step and revision)
	jmp	@@7
@@6:
	clr	bx	; unknown Cyrix chip
@@7:
	sti
	mov	ax,bx
	popf
	ret
	endp

	END
