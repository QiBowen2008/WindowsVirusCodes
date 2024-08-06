; -----------------------------------------------------------------------------
; HEADER.ASH	Global options, memory model definition and common includes
;		for TMi0SDGL
;
; Copyright(c) 1994,95 by B-coolWare.  Written by Bobby Z.
; -----------------------------------------------------------------------------

IFNDEF	__TINY__
 IFNDEF  __SMALL__
  IFNDEF  __MEDIUM__
   IFNDEF  __COMPACT__
    IFNDEF  __LARGE__
     IFNDEF  __HUGE__
      .err Must set memory model.
     ELSE
        MODEL	HUGE,PASCAL
     ENDIF
    ELSE
	MODEL	LARGE,PASCAL
    ENDIF
   ELSE
	MODEL	COMPACT,PASCAL
   ENDIF
  ELSE
	MODEL	MEDIUM,PASCAL
  ENDIF
 ELSE
	MODEL	SMALL,PASCAL
 ENDIF
ELSE
	MODEL	TINY,PASCAL
ENDIF

	LOCALS  @@
	JUMPS

	INCLUDE	UNIDEF.INC
