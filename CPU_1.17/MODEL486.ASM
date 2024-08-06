; -----------------------------------------------------------------------------
; MODEL486.ASM	Code to determine 486 chip step and model without using
;		CPUID instruction (requires processor to be in real mode).
;
; Copyright(c) by Ilya Tumanov of 2:5030/82.6@fidonet
; Included in TMi0SDGL by author's permission.
; -----------------------------------------------------------------------------
;
; The following code uses CPU reset sequence to obtain CPU internal ID.
; Thus it won't work from protected mode and on some BIOSes it may return
; incorrect results if the BIOS code destroys registers during initialization
; before passing control back to our code (Award BIOS is known to do so, while
; AMI and MR returns needed register unchanged).
;
; Returned code is (afaik) formatted like this:
;
; bits 15-12: reserved
;  bits 11-8: CPU family ( 3 for 386, 4 for 486, 5 for P5, etc.)
;   bits 7-4: CPU model  ( see table below )
;   bits 3-0: CPU step/submodel
;
; CPU models defined for now (this list maybe not complete):
;
; 0,1: DX
; 2  : SX
; 3  : DX2/Overdrive
; 4  : SL
; 5  : SX2
; 6  : ???
; 7  : DX2WB
; 8  : DX4
;
; other codes are currently undefined (that is, reserved). These codes are
; also partially valid for 386 CPUs (excluding DX2, SX2 and DX4).
;
; Note that the table is valid only for Intel/AMD/UMC chips. Cyrix/TI chips
; also return such info, but its meaning is different and unknown to me.
;

	INCLUDE	HEADER.ASH

	.CODE

	.286

	PUBLIC	get486Model

save_sp	dw	?	; we'll save SP here
save_bp dw	?	; saved BP
save_ss	dw	?	; saved SS
save_ax dw	?	; save ax after reset

get486Model	proc
; all GPRs destroyed, except SP and BP. Segment registers are preserved.

	in	al,21h		; get IRQ mask
	push	ax		; and save it on stack
	push	ds es		; DS and ES are saved on stack too
	mov	cs:save_sp,sp	; save stack offset
	mov	cs:save_ss,ss	; and segment
	mov	cs:save_bp,bp	; save BP for the case it gets corrupted
	push	0040h		; segment addr of BIOS RAM work area
	pop	es		; put in es
	mov	ax,offset quit	; get return addr offset after reset
	mov	es:[67h],ax	; put in return address area in BIOS RAM
	mov	ax,cs		; get our current code segment
	mov	es:[69h],ax	; put in return address area in BIOS RAM

	mov	al,08Fh		; address of CMOS RAM to use  (8FH)
	out	70h,al		; set CMOS RAM address latch
	REPT	3
	jmp short $+2
	ENDM
	mov	al,5		; signal that this reset is a return
	out	71h,al		; from protected mode-output to CMOS RAM
	REPT	3
	jmp short $+2
	ENDM
	mov	al,0FEh		; load shutdown command for the 8042
	out	64h,al		; issue the reset
	hlt			; halt while waiting for hardware reset
	jmp short $-1		; jump on halt in case of interrupt

quit:

; processor reset at this point, interrupts are disabled and IRQs masked.
; restoring segment registers.

	mov	cs:save_ax,ax
	mov	sp,cs:save_ss	; restoring stack first of all
	mov	ss,sp
	mov	sp,cs:save_sp
	mov	bp,cs:save_bp
	pop	es ds		; then restoring segment registers
	pop	ax		; al = saved IRQ mask
	out	21h,al		; restore IRQ mask

	sti			; reenable interrupts

	mov	ax,cs:save_ax
	test	ax,ax		; ax = 0 means CPU is OK, else it failed
				; internal test
	mov	ax,dx		; DX contains the same code as returned in
				; AX by CPUID with EAX=1
	ret
	endp

	END
