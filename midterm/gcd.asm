List p=18f4520
    ;	init
    #include<p18f4520.inc>
    ; == config == ;
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00

        aH EQU 0x00
        aL EQU 0x01
        bH EQU 0x02
        bL EQU 0x03
        tmpH EQU 0x04
        tmpL EQU 0x05
        dividendH EQU 0x10
        dividendL EQU 0x11
        divisorH EQU 0x12
        divisorL EQU 0x13
        remainderH EQU 0x14
        remainderL EQU 0x15
        quotientH EQU 0x16
        quotientL EQU 0x17
        divCount EQU 0x18

        initData MACRO position, value
            MOVLW value
            MOVWF position
        ENDM
        RL16bits MACRO high, low
            RLCF low
            RLCF high
        ENDM
        SUB16bits MACRO subEndH, subEndL, suberH, suberL
            MOVF suberL, W
            SUBWF subEndL, F
            MOVF suberH, W
            SUBWFB subEndH, F
        ENDM
        ADD16bits MACRO addEndH, addEndL, adderH, adderL
            MOVF adderL, W
            ADDWF addEndL, F
            MOVF adderH, W
            ADDWFC addEndH, F
        ENDM

        ; while b != 0
        ;   temp = b
        ;   b = a mod b
        ;   a = temp
        ; return a

        initData aH, 0x48
        initData aL, 0x00
        initData bH, 0x18
        initData bL, 0x00

    gcd_loop:
        initData tmpH, 0x00
        initData tmpL, 0x00
        SUB16bits tmpH, tmpL, bH, bL ; 0 - b
        BTFSC STATUS, 0x00 ; skip if c is 0
        BRA gcd_end ; >= 0 ; b == 0
        ; < 0 ; b != 0

        MOVFF aH, dividendH
        MOVFF aL, dividendL
        MOVFF bH, divisorH
        MOVFF bL, divisorL
        RCALL   division_16bits
        MOVFF bH, aH
        MOVFF bL, aL
        MOVFF remainderH, bH
        MOVFF remainderL, bL

        BRA gcd_loop

division_16bits:
    initData divCount, 0x10
    CLRF remainderH
    CLRF remainderL
    CLRF quotientH
    CLRF quotientL
    
    divLoop:
        RL16bits dividendH, dividendL
        RL16bits remainderH, remainderL
        RL16bits quotientH, quotientL
        BSF quotientL, 0
        
        SUB16bits remainderH, remainderL, divisorH, divisorL
        BTFSC STATUS, 0x00 ; skip if c is 0
        BRA divContinue ; >= 0
        
        ; < 0 ; restore
        ADD16bits remainderH, remainderL, divisorH, divisorL
        BCF quotientL, 0
	divContinue:
	    DECFSZ divCount ; skip if 0
	    BRA divLoop
RETURN

    gcd_end:
        NOP
    END