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
    RL16bits MACRO high, low
        RLCF low
        RLCF high
    ENDM
    RR16bits MACRO high, low
        RRCF high
        RRCF low
    ENDM
    SUB16bits MACRO subEndH, subEndL, suberH, suberL
        MOVF suberL, W
        SUBWF subEndL, F
        MOVF suberH, W
        SUBWFB subEndH, F
    ENDM
    tmp EQU 0x40
    count EQU 0x42

    LFSR 0, 0x020
    initData POSTINC0, 0x92
    initData POSTINC0, 0x76
    initData POSTINC0, 0x19
    initData POSTINC0, 0x04
    LFSR 0, 0x030
    initData POSTINC0, 0x78
    initData POSTINC0, 0x13
    initData POSTINC0, 0x61
    initData POSTINC0, 0x92

    MOVF 0x33, w
    ADDWF 0x23, w
    BTFSC STATUS, 0x01
    ADDLW 0x06
    ADDWF 0x23, w
    BTFSS STATUS, 0x00
    BRA 1_next
    ADDLW 0x60
    INCF 0x23

    1_next:
        MOVWF 0x04

    MOVF 0x32, w
    ADDWF 0x22, w
    BTFSC STATUS, 0x01
    ADDLW 0x06
    ADDWF 0x22, w
    BTFSS STATUS, 0x00
    BRA 2_next
    ADDLW 0x60
    INCF 0x22

    2_next:
        MOVWF 0x03

    MOVF 0x31, w
    ADDWF 0x21, w
    BTFSC STATUS, 0x01
    ADDLW 0x06
    ADDWF 0x21, w
    BTFSS STATUS, 0x00
    BRA 3_next
    ADDLW 0x60
    INCF 0x20

    3_next:
        MOVWF 0x01

    MOVF 0x30, w
    ADDWF 0x20, w
    BTFSC STATUS, 0x01
    ADDLW 0x06
    ADDWF 0x20, w
    BTFSS STATUS, 0x00
    BRA 4_next
    ADDLW 0x60

    4_next:
        MOVWF 0x00



    NOP

    END