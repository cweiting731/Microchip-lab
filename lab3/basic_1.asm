List p=18f4520
    ;	init
    #include<p18f4520.inc>
    ; == variable == ;
	SRC EQU TRISA
	TMP EQU 0x02
        ; 00101101 ; 11010010 ; 01111000 ; 10110100

    ; == config == ;
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
    ; == init data == ;
	MOVLW 0xA6
	MOVWF SRC
	
    ; 00101101 -> 11010010
    ; rotate 4
    ; value = (byte)(((value & 0xaa) >> 1) | ((value & 0x55) << 1));
	MOVLW b'10101010'
	ANDWF SRC, W
	MOVWF TMP
	RRNCF TMP
	
	MOVLW b'01010101'
	ANDWF SRC, W
	RLNCF WREG
	
	IORWF TMP, W
	MOVWF SRC
	
    ; value = (byte)(((value & 0xcc) >> 2) | ((value & 0x33) << 2));
	MOVLW b'11001100'
	ANDWF SRC, W
	MOVWF TMP
	RRNCF TMP
	RRNCF TMP
	
	MOVLW b'00110011'
	ANDWF SRC, W
	RLNCF WREG
	RLNCF WREG
	
	IORWF TMP, W
	MOVWF SRC

    ; return (byte)((value >> 4) | (value << 4));
	SWAPF SRC
	
	NOP
	END