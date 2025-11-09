List p=18f4520
    ;	init
    #include<p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
	; init load data
	MOVLW b'11111111' ; testcast 1 = b'00001000', testcast 2 = b'01101000'
	MOVWF 0x00 ; [0x00] = W
	MOVWF 0x01 ; [0x01] = tmp = W
	CLRF 0x10 ; clz = 0
	
	loop:
	BTFSC 0x01, 7 ; skip if the MSB of tmp is 0
	BRA done
	
	RLNCF 0x01 ; rotate left f (no carry); store in f
	INCF 0x01 ; set the LSB of tmp 1
	INCF 0x10 ; clz++
	BRA loop
	
	done:
	
	END

