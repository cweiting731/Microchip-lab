List p=18f4520
    ;	init
    #include<p18f4520.inc>
    ; == variable == ;
	; x * y
	; 1 : - 
	; 0 : +
	x EQU 0x00
	x_sig EQU 0x04
	y EQU 0x01
	y_sig EQU 0x05
	ans EQU 0x02
	count EQU 0x07
    
    ; == config == ;
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
	
    ; == init data == ;
	MOVLW 0xFE
	MOVWF x
	MOVLW 0xFC
	MOVWF y
	
	CLRF x_sig
	CLRF y_sig
	CLRF ans
	
    ; == check x and y is + or - == ;
    x_sig_test:
	BTFSS x, d'7' ; skip if MSB is 1
	BRA y_sig_test ; do not do the 2's
	
	; set 1 if MSB is 1 ; -
	BSF x_sig, d'0'
	; -1 ; reverse
	DECF x ;
	MOVLW 0xFF ; set mask
	XORWF x, F ; reverse x
	
    y_sig_test:
	BTFSS y, d'7' ; skip if MSB is 1
	BRA mul
	
	; set 1 if MSB is 1 ; -
	BSF y_sig, d'0'
	; -1 ; reverse
	DECF y;
	MOVLW 0xFF ; set mask
	XORWF y, F ; reverse y
	
    mul:
	MOVLW 0x04
	MOVWF count
	mulLoop: 
	    BTFSS y, d'0' ; skip if LSB is 1
	    BRA mulSkip
	    
	    ; add to ans
	    MOVF x, W
	    ADDWF ans, F
	    
	    mulSkip:
		RLNCF x
		RRNCF y
    
	    ; count--
	    DECFSZ count, F
	    BRA mulLoop
	    BRA checkAnsSig
	    
    checkAnsSig:
	MOVF x_sig, W
	XORWF y_sig, W
	BTFSS WREG, d'0' ; skip if W's LSB is 1 (-)
	BRA done ; = 0
	
	; (-) do 2's 
	; reverse ; + 1
	MOVLW 0xFF ; mask
	XORWF ans, F
	INCF ans
	
    done:
	NOP
	END

