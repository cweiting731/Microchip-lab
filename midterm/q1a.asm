List p=18f4520
    ;	init
    #include<p18f4520.inc>
    ; == config == ;
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00

    initData MACRO position, value
        MOVLW value
        MOVWF position
    ENDM

    startH EQU 0x00
    startL EQU 0x01
    sigH EQU 0x03
    sigE EQU 0x04
    ans EQU 0x10
    ANS EQU 0x02
 
    initData startH, 0x63
    initData startL, 0xC6

    ; 0
	MOVLW 0x01
	ANDWF startL, W ; get LSB of SRC -> W
	RLNCF ANS ; left rotate ANS 
	IORWF ANS, F ; store LSB in ANS LSB
	RRNCF startL ; right roate SRC
	
	; 1
	MOVLW 0x01
	ANDWF startL, W ; get LSB of SRC -> W
	RLNCF ANS ; left rotate ANS 
	IORWF ANS, F ; store LSB in ANS LSB
	RRNCF startL ; right roate SRC
	
	; 2
	MOVLW 0x01
	ANDWF startL, W ; get LSB of SRC -> W
	RLNCF ANS ; left rotate ANS 
	IORWF ANS, F ; store LSB in ANS LSB
	RRNCF startL ; right roate SRC
	
	; 3
	MOVLW 0x01
	ANDWF startL, W ; get LSB of SRC -> W
	RLNCF ANS ; left rotate ANS 
	IORWF ANS, F ; store LSB in ANS LSB
	RRNCF startL ; right roate SRC
	
	; 4
	MOVLW 0x01
	ANDWF startL, W ; get LSB of SRC -> W
	RLNCF ANS ; left rotate ANS 
	IORWF ANS, F ; store LSB in ANS LSB
	RRNCF startL ; right roate SRC
	
	; 5
	MOVLW 0x01
	ANDWF startL, W ; get LSB of SRC -> W
	RLNCF ANS ; left rotate ANS 
	IORWF ANS, F ; store LSB in ANS LSB
	RRNCF startL ; right roate SRC
	
	; 6
	MOVLW 0x01
	ANDWF startL, W ; get LSB of SRC -> W
	RLNCF ANS ; left rotate ANS 
	IORWF ANS, F ; store LSB in ANS LSB
	RRNCF startL ; right roate SRC
	
	; 7
	MOVLW 0x01
	ANDWF startL, W ; get LSB of SRC -> W
	RLNCF ANS ; left rotate ANS 
	IORWF ANS, F ; store LSB in ANS LSB
	RRNCF startL ; right roate SRC
	MOVFF ANS, startL
	RRNCF startL
    BCF startH, 0x07
    BCF startL, 0x07

    MOVF startH, w
    CPFSEQ startL ; skip if equal
    BRA q1a_not_palindrome ; not palindrome
    BRA q1a_is_palindrome ; is palindrome

    q1a_not_palindrome:
        initData ans, 0x00
        BRA q1a_done
    q1a_is_palindrome:
        initData ans, 0xFF
    
    q1a_done:
        NOP
    
    END