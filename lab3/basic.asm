List p=18f4520
    ;	init
    #include<p18f4520.inc>
    ; == variable == ;
	SRC EQU TRISA
	ANS EQU 0x01
    ; 00101101 ; 11010010 ; 01111000 ; 10110100
    ; == config == ;
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
    ; == init data == ;
	MOVLW 0xCC
	MOVWF SRC
	CLRF ANS
	
	; W = 0x01 ; set mask
	
	; 0
	MOVLW 0x01
	ANDWF SRC, W ; get LSB of SRC -> W
	RLNCF ANS ; left rotate ANS 
	IORWF ANS, F ; store LSB in ANS LSB
	RRNCF SRC ; right roate SRC
	
	; 1
	MOVLW 0x01
	ANDWF SRC, W ; get LSB of SRC -> W
	RLNCF ANS ; left rotate ANS 
	IORWF ANS, F ; store LSB in ANS LSB
	RRNCF SRC ; right roate SRC
	
	; 2
	MOVLW 0x01
	ANDWF SRC, W ; get LSB of SRC -> W
	RLNCF ANS ; left rotate ANS 
	IORWF ANS, F ; store LSB in ANS LSB
	RRNCF SRC ; right roate SRC
	
	; 3
	MOVLW 0x01
	ANDWF SRC, W ; get LSB of SRC -> W
	RLNCF ANS ; left rotate ANS 
	IORWF ANS, F ; store LSB in ANS LSB
	RRNCF SRC ; right roate SRC
	
	; 4
	MOVLW 0x01
	ANDWF SRC, W ; get LSB of SRC -> W
	RLNCF ANS ; left rotate ANS 
	IORWF ANS, F ; store LSB in ANS LSB
	RRNCF SRC ; right roate SRC
	
	; 5
	MOVLW 0x01
	ANDWF SRC, W ; get LSB of SRC -> W
	RLNCF ANS ; left rotate ANS 
	IORWF ANS, F ; store LSB in ANS LSB
	RRNCF SRC ; right roate SRC
	
	; 6
	MOVLW 0x01
	ANDWF SRC, W ; get LSB of SRC -> W
	RLNCF ANS ; left rotate ANS 
	IORWF ANS, F ; store LSB in ANS LSB
	RRNCF SRC ; right roate SRC
	
	; 7
	MOVLW 0x01
	ANDWF SRC, W ; get LSB of SRC -> W
	RLNCF ANS ; left rotate ANS 
	IORWF ANS, F ; store LSB in ANS LSB
	RRNCF SRC ; right roate SRC
	
	MOVFF ANS, SRC
	
	; after the function
	; SRC [0x000] will store the original testcast
	; ANS [0x001] will store the reversal testcast
	
	NOP
	END