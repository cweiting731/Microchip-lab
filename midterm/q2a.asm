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

    LFSR 0, 0x000
    LFSR 1, 0x001
    LFSR 2, 0x002

    initData INDF0, 0x04
    initData INDF1, 0x0F

    MOVF POSTINC0, w
    ADDWF POSTINC1, w
    MOVWF POSTINC2

    MOVF POSTINC0, w
    ADDWF POSTINC1, w
    MOVWF POSTINC2

    MOVF POSTINC0, w
    ADDWF POSTINC1, w
    MOVWF POSTINC2

    MOVF POSTINC0, w
    ADDWF POSTINC1, w
    MOVWF POSTINC2

    MOVF POSTINC0, w
    ADDWF POSTINC1, w
    MOVWF POSTINC2

    MOVF POSTINC0, w
    ADDWF POSTINC1, w
    MOVWF POSTINC2
    NOP
    END