#include "xc.inc"
GLOBAL _mul_extended
PSECT mytext, local, class=CODE, reloc=2
; == variable == ;
 nH EQU 0x10
 nL EQU 0x11
 mH EQU 0x12
 mL EQU 0x13
 
 n_sig EQU 0x20
 m_sig EQU 0x21
 
 ansHH EQU 0x30
 ansHL EQU 0x31
 ansLH EQU 0x32
 ansLL EQU 0x33
 
 tmpHH EQU 0x40
 tmpHL EQU 0x41
 tmpLH EQU 0x42
 tmpLL EQU 0x43

; == MACRO == ;
initData MACRO position, value
    MOVLW value
    MOVWF position
ENDM
ADD16bits MACRO addEndH, addEndL, adderH, adderL
    MOVF adderL, W
    ADDWF addEndL, F
    MOVF adderH, W
    ADDWFC addEndH, F
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

_mul_extended:
    MOVFF 0x02, nH
    MOVFF 0x01, nL
    MOVFF 0x04, mH
    MOVFF 0x03, mL
    CLRF n_sig
    CLRF m_sig
    
    n_sig_test:
	BTFSS nH, 0x07 ; skip if -
	BRA m_sig_test; +
	; - ; 2's
	Reverse16bits nH, nL
	INCF16bits nH, nL
	INCF n_sig
    
    m_sig_test:
	BTFSS mH, 0x07 ; skip if -
	BRA mul
	; - ; 2's
	Reverse16bits mH, mL
	INCF16bits mH, mL
	INCF m_sig
    
    mul:
	MOVF mL, W
	MULWF nL
	MOVFF PRODH, tmpLH
	MOVFF PRODL, tmpLL
	CLRF tmpHH
	CLRF tmpHL
	ADD32bits ansHH, ansHL, ansLH, ansLL, tmpHH, tmpHL, tmpLH, tmpLL
	
	MOVF mL, W
	MULWF nH
	MOVFF PRODH, tmpHL
	MOVFF PRODL, tmpLH
	CLRF tmpHH
	CLRF tmpLL
	ADD32bits ansHH, ansHL, ansLH, ansLL, tmpHH, tmpHL, tmpLH, tmpLL
	
	MOVF mH, W
	MULWF nL
	MOVFF PRODH, tmpHL
	MOVFF PRODL, tmpLH
	CLRF tmpHH
	CLRF tmpLL
	ADD32bits ansHH, ansHL, ansLH, ansLL, tmpHH, tmpHL, tmpLH, tmpLL
	
	MOVF mH, W
	MULWF nH
	MOVFF PRODH, tmpHH
	MOVFF PRODL, tmpHL
	CLRF tmpLH
	CLRF tmpLL
	ADD32bits ansHH, ansHL, ansLH, ansLL, tmpHH, tmpHL, tmpLH, tmpLL
	
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