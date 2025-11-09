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

    count EQU 0x07
    innerCount EQU 0x08
    ans EQU 0x10
    target EQU 0x05

    LFSR 0, 0x00
    LFSR 1, 0x01
    initData POSTINC0, 0x05
    initData POSTINC0, 0x05
    initData POSTINC0, 0x05
    initData POSTINC0, 0x05
    initData POSTINC0, 0x05
    initData POSTINC0, 0x0A
    LFSR 0, 0x00

    initData count, 0x04

    q3a_loop:
        MOVFF count, innerCount
	MOVFF FSR0L, FSR1L
	INCF FSR1L

        q3a_inner_loop:
            MOVF POSTINC1, w
            ADDWF INDF0, w
            CPFSEQ target ; skip if equal
            BRA q3a_inner_loop_skip ; not equal
            INCF ans ; equal

            q3a_inner_loop_skip:

            DECFSZ innerCount, f
            BRA q3a_inner_loop

        MOVF POSTINC0, f
        DECFSZ count, F
		BRA q3a_loop

    NOP
    END