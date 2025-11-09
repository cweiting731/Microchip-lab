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

    LFSR 0, 0x200
    initData POSTINC0, 0x01
    initData POSTINC0, 0x02
    initData POSTINC0, 0x03
    LFSR 0, 0x210
    initData POSTINC0, 0x04
    initData POSTINC0, 0x05
    initData POSTINC0, 0x06
    LFSR 0, 0x220
    initData POSTINC0, 0x07
    initData POSTINC0, 0x08
    initData POSTINC0, 0x09

    LFSR 0, 0x201
    LFSR 1, 0x210
    MOVF


    END