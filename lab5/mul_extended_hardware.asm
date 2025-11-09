#include "xc.inc"
GLOBAL _mul_extended_hardware
PSECT mytext, local, class=CODE, reloc=2
; == variable == ;
 nHH EQU 0x10
 nHL EQU 0x11
 nLH EQU 0x12
 nLL EQU 0x13
 mH EQU 0x14
 mL EQU 0x15
 
 n_sig EQU 0x20
 m_sig EQU 0x21
 
 ansHH EQU 0x30
 ansHL EQU 0x31
 ansLH EQU 0x32
 ansLL EQU 0x33
 
 count EQU 0x35

; == MACRO == ;
initData MACRO position, value
    MOVLW value
    MOVWF position
ENDM
ADD32bits MACRO addEndHH, addEndHL, addEndLH, addEndLL, adderHH, adderHL, adderLH, adderLL
    MOVF adderLL, W
    ADDWF addEndLL, F
    MOVF adderLH, W
    ADDWFC addEndLH, F
    MOVF adderHL, W
    ADDWFC addEndHL, F
    MOVF adderHH, W
    ADDWFC addEndHH, F
ENDM
RL32bits MACRO HH, HL, LH, LL
    RLCF LL, F
    RLCF LH, F
    RLCF HL, F
    RLCF HH, F
ENDM
RR16bits MACRO H, L
    RRCF H, F
    RRCF L, F
ENDM
INCF16bits MACRO positionH, positionL
    INCF positionL
    BTFSC STATUS, 0x02 ; skip if positionL != 0
    INCF positionH
ENDM
Reverse16bits MACRO positionH, positionL
    MOVLW 0xFF
    XORWF positionH, F
    XORWF positionL, F
ENDM
INCF32bits MACRO HH, HL, LH, LL
    INCF LL
    BTFSC STATUS, 0x02 ; skip if positionL != 0
    INCF LH
    BTFSC STATUS, 0x02 ; skip if positionL != 0
    INCF HL
    BTFSC STATUS, 0x02 ; skip if positionL != 0
    INCF HH
ENDM

_mul_extended_hardware:
    MOVFF 0x02, nLH
    MOVFF 0x01, nLL
    MOVFF 0x04, mH
    MOVFF 0x03, mL
    CLRF nHH
    CLRF nHL
    CLRF n_sig
    CLRF m_sig
    
    n_sig_test:
	BTFSS nLH, 0x07 ; skip if -
	BRA m_sig_test; +
	; - ; 2's
	Reverse16bits nLH, nLL
	INCF16bits nLH, nLL
	INCF n_sig
    
    m_sig_test:
	BTFSS mH, 0x07 ; skip if -
	BRA mul
	; - ; 2's
	Reverse16bits mH, mL
	INCF16bits mH, mL
	INCF m_sig
    
    mul:
	initData count, 0x10
	
	mul_loop:
	    BTFSS mL, 0x00 ; skip if LSB is 1
	    BRA mul_skip
	    
	    ; add to ans
	    ADD32bits ansHH, ansHL, ansLH, ansLL, nHH, nHL, nLH, nLL
	    
	    mul_skip:
		RL32bits nHH, nHL, nLH, nLL
		BCF nLL, 0x00
		RR16bits mH, mL
	    
	    DECFSZ count, F
	    BRA mul_loop
	
	checkAnsSig:
	    MOVF m_sig, W
	    XORWF n_sig, W
	    BTFSS WREG, 0x00 ; skip if -
	    BRA done ; +
	    ; - ; 2's
	    Reverse16bits ansHH, ansHL
	    Reverse16bits ansLH, ansLL
	    INCF32bits ansHH, ansHL, ansLH, ansLL
	
	done:
	    MOVFF ansLL, 0x01
	    MOVFF ansLH, 0x02
	    MOVFF ansHL, 0x03
	    MOVFF ansHH, 0x04
	
RETURN