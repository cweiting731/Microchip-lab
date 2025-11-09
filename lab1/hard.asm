List p=18f4520
    ;	init
    #include<p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
	; init load data
	MOVLW b'11111111' ; testcast 1: b'00001000', testcast 2: b'01101000', testcast 3: b'00000000', testcast 4: b'11111111'
	MOVWF 0x00 ; [0x00] = W
	MOVWF 0x01 ; [0x01] = x = W
	CLRF 0x10 ; [0x10] = count = 0
	
	fourTest:
	MOVLW b'00010000'
	CPFSLT 0x01 ; x has four 0; skip if f < W
	GOTO twoTest ; x hasn't four 0
	
	; count += 4
	MOVLW d'4'
	ADDWF 0x10 ; store in f
	; rotate 4 times
	RLNCF 0x01
	RLNCF 0x01
	RLNCF 0x01
	RLNCF 0x01
	
	twoTest:
	MOVLW b'01000000'
	CPFSLT 0x01 ; x has two 0; skip if f < w
	GOTO oneTest
	
	; count += 2
	MOVLW d'2'
	ADDWF 0x10 ; store in f
	; rotate 2 times
	RLNCF 0x01
	RLNCF 0x01
	
	oneTest:
	BTFSC 0x01, 7; skip if bit 0
	GOTO zeroTest
	INCF 0x10 ; x has one 0, count++
	RLNCF 0x01 ; rotate 1 times
	
	zeroTest: 
	TSTFSZ 0x01; skip if f == 0
	GOTO done_hard
	INCF 0x10 ; x == 0; count++
	
	done_hard:

    END