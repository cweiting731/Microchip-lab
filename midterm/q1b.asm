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

    HH EQU 0x00
    HL EQU 0x01
    LH EQU 0x02
    LL EQU 0x03
    TMPH EQU 0x05
    TMPL EQU 0x06
    ANSH EQU 0x07
    ANSL EQU 0x08
    count EQU 0x09
    ans EQU 0x10
    tmp EQU 0x04

    initData HH, 0x0C
    initData HL, 0x22
    initData LH, 0x22
    initData LL, 0xC0

    MOVLW b'00001111'
    ANDWF LH, w
    IORWF tmp, f

    MOVLW b'11110000'
    ANDWF LL, w
    IORWF tmp, f

    MOVF tmp, w
    CPFSEQ HL ; skip if equal
    BRA q1b_notPalidrome

    MOVLW b'00001111'
    ANDWF LL, f
    MOVF LL, w
    CPFSEQ HH ; skip if equal
    BRA q1b_notPalidrome
    
    q1b_isPalidrome:
        initData ans, 0xFF
        BRA q1b_done

    q1b_notPalidrome:
        initData ans, 0x00
    q1b_done:
        NOP

    END