List p=18f4520
    ;	init
    #include<p18f4520.inc>
    ; == config == ;
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
    ; == variable == ;
	xH   EQU 0x02
	xL   EQU 0x03
	yH   EQU 0x04
	yL   EQU 0x05
 
	andH EQU 0x00
	andL EQU 0x01
	mulH EQU 0x10
	mulL EQU 0x11
    ; == macro == ;
    initData MACRO position, value
	MOVLW value
	MOVWF position
    ENDM
    
    And_Mul MACRO xh, xl, yh, yl
	; andH = xh & yh
	MOVF xh, W
	ANDWF yh, W
	MOVWF andH
	
	; andL = xl & yl
	MOVF xl, W
	ANDWF yl, W
	MOVWF andL
	
	; [mulH, mulL] = andH * andL
	MOVF andH, W
	MULWF andL
	MOVFF PRODH, mulH
	MOVFF PRODL, mulL
    ENDM
    
    ; == main function == ;
	initData xH, 0x50
	initData xL, 0x6F
	initData yH, 0x3A
	initData yL, 0xBC
	
	And_Mul xH, xL, yH, yL
	NOP
    END
    
	
	


