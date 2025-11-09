List p=18f4520
    ;	init
    #include<p18f4520.inc>
    ; == config == ;
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
    ; == variable == ;
	DividendH   EQU 0x00
	DividendL   EQU 0x01
	DivisorH    EQU 0x02
	DivisorL    EQU 0x03
	QuotientH   EQU 0x10
	QuotientL   EQU 0x11
	RemainderH  EQU 0x12
	RemainderL  EQU 0x13

	count       EQU 0x15
    ; == macro == ;
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

    ; == main function == ;
    initData DividendH, 0xFA
    initData DividendL, 0x9F
    initData DivisorH, 0x03
    initData DivisorL, 0x45
    
    RCALL division
    RCALL done

    
    division:
	; == init setting == ;
	initData count, 0x01
	CLRF QuotientH
	CLRF QuotientL
	MOVFF DividendH, RemainderH
	MOVFF DividendL, RemainderL
	
	; == preprocess divisor == ;
	divisorPreprocess:
	    BTFSC DivisorH, d'7'
	    BRA divisionLoop
	    RL16bits DivisorH, DivisorL
	    BCF DivisorL, d'0'
	    INCF count
	    BRA divisorPreprocess
	    
	divisionLoop:
	    ; == compare remainder and divisor == ;
	    MOVF RemainderH, W
	    CPFSGT DivisorH ; skip if DivisorH > RemainderH
	    BRA highEquleDetected
	    BRA set0
	    
	    highEquleDetected:
		CPFSEQ DivisorH ; skip if DivisorH = RemainderH
		BRA set1 ; DivisorH < RemainderH
		
	    MOVF RemainderL, W
	    CPFSGT DivisorL ; skip if DivisorL > RemainderL
	    BRA set1
	    BRA set0

	    set1:
		SUB16bits RemainderH, RemainderL, DivisorH, DivisorL
		RL16bits QuotientH, QuotientL
		BSF QuotientL, d'0'
		BRA nextBit
	    set0:
		RL16bits QuotientH, QuotientL
		BCF QuotientL, d'0'
	    nextBit:
		RR16bits DivisorH, DivisorL
		BCF DivisorH, d'7'
		DECFSZ count, F
		BRA divisionLoop

    RETURN
    
    done:
	NOP
    END