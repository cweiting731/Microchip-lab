List p=18f4520
    ;	init
    #include<p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
	; init load data
	MOVLW 0xC2 ; testcast 1 = 0x03, testcast 2 = 0x08
	MOVWF 0x00 ; x1
	MOVLW 0x1E ; testcast 1 = 0x02, testcast 2 = 0x08
	MOVWF 0x01 ; x2
	MOVLW 0xF7 ; testcast 1 = 0xB5, testcast 2 = 0x0F
	MOVWF 0x02 ; y1
	MOVLW 0xBF ; testcast 1 = 0x04, testcast 2 = 0x02
	MOVWF 0x03 ; y2
	
	; W = WREG
	; A1 = x1 + x2
	MOVF 0x00, W ; W = x1 (default f(1))
	ADDWF 0x01, W ; W = x2 + W (default f(1))
	MOVWF 0x10 ; A1 = W
	
	; A2 = y1 - y2
	MOVF 0x03, W ; W = y2
	SUBWF 0x02, W ; W = y1 - W
	MOVWF 0x11 ; A2 = W
	
	; A1 > A2 -> 0xFF; A1 <= A2 -> 0x01
	CPFSGT 0x10; Compare f with W, skip if f > W (f = A1, W = A2)
	GOTO one ; A1 <= A2 -> 0x01
	GOTO full ; A1 > A2 -> 0xFF
	
	one: 
	MOVLW 0x01 ; W = 0x01
	GOTO done_basic
	
	full: 
	MOVLW 0xFF ; W = 0xFF

	done_basic:
	MOVWF 0x20 ; [0x020] = W
	
	END

