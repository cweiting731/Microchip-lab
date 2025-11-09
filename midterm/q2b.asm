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

    count EQU 0x34
    innerCount EQU 0x36
    tmp EQU 0x35

    LFSR 0, 0x010
    initData POSTINC0, 0x01
    initData POSTINC0, 0x07
    initData POSTINC0, 0x04
    LFSR 0, 0x020
    initData POSTINC0, 0x02
    initData POSTINC0, 0x03
    initData POSTINC0, 0x09
    LFSR 0, 0x030
    initData POSTINC0, 0x08
    initData POSTINC0, 0x06
    initData POSTINC0, 0x05
    LFSR 0, 0x013
    LFSR 1, 0x020
    MOVFF POSTINC1, POSTINC0
    MOVFF POSTINC1, POSTINC0
    MOVFF POSTINC1, POSTINC0
    LFSR 1, 0x030
    MOVFF POSTINC1, POSTINC0
    MOVFF POSTINC1, POSTINC0
    MOVFF POSTINC1, POSTINC0

    initData count, 0x09
    LFSR 0, 0x000

    q2b_loop:
        LFSR 2, 0x010
        LFSR 1, 0x010
        initData innerCount, 0x09
        q2b_inner:
            MOVF INDF1, w
            CPFSGT INDF2 ; skip if f > w
            BRA q2b_inner_skip

            ; swap
            MOVFF FSR1L, FSR2L

            q2b_inner_skip:
                MOVF POSTINC1, f
            
            DECFSZ innerCount, f
            BRA q2b_inner

        MOVFF INDF2, POSTINC0
        initData POSTINC2, 0xFF

        DECFSZ count, f
        BRA q2b_loop

    NOP
    END