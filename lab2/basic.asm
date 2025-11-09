List p=18f4520
    ;	init
    #include<p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
	; init data
	LFSR 0, 0x120 ; FSR0 -> 0x120
	MOVLW 0xA2 ; testcast 1 = 0x02, testcast 2 = 0xFE
	MOVWF POSTINC0 ; [0x120] = W ; FSR0 -> 0x121
	MOVLW 0xD9 ; testcast 1 = 0x03, testcast 2 = 0xEE
	MOVWF POSTINC0 ; [0x121] = W ; FSR0 -> 0x122
	
	; record execute index
	MOVLW 0x21 ; W = 0x21
	MOVWF 0xF0 ; 0x0F0 (i) = W
	
	; FSR0 -> addr
	LFSR 1, 0x120 ; FSR1 -> addr - 2
	LFSR 2, 0x121 ; FSR2 -> addr - 1
    loop: 
	; for i < 26
	MOVLW 0x26
	CPFSLT 0xF0 ; if i < 0x26
	    BRA done ; i >= 26, go to end
	
	INCF 0xF0 ; i++ 
	MOVF POSTINC2, W ; W = FSR2 (addr - 1)
	BTFSC 0xF0, 0 ; bit test 0 of i, if 0, then skip
	BRA odd
	
	even: 
	    ADDWF POSTINC1, W ; W = [addr - 2] + [addr - 1]
	    BRA loopFinally
	odd:
	    SUBWF POSTINC1, W ; W = [addr - 2] - [addr - 1]
	
	loopFinally:
	    MOVWF POSTINC0 ; [addr] = W
	    BRA loop
	
    done:
	NOP
	
	END


