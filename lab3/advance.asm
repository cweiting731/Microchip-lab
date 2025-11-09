List p=18f4520
    ;	init
    #include<p18f4520.inc>
    ; == variable == ;
	minuendH EQU 0x00
	minuendL EQU 0x01
	subtrahendH EQU 0x10
	subtrahendL EQU 0x11
	ansH EQU 0x20
	ansL EQU 0x21
    
    ; == config == ;
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
	
    ; == init data == ;
	MOVLW 0x10
	MOVWF minuendH
	MOVLW 0x00
	MOVWF minuendL
	
	MOVLW 0x00
	MOVWF subtrahendH
	MOVLW 0x01
	MOVWF subtrahendL
	
	; SUBWF -> f - w
    ; == sub L == ;
	SUBWF minuendL, W
	MOVWF ansL
	
    ; == minuendL < subtrahendL, borrow 1 == ;
	BTFSS STATUS, C ; skip if 1 ; STATUS (5 bits) = N, OV, Z, DC, C
    	DECF minuendH ; C = 0
	
    ; == sub H == ;
	MOVF subtrahendH, W
	SUBWF minuendH, W
	MOVWF ansH
	
	NOP
	END

	
