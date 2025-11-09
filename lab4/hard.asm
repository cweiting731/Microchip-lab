List p=18f4520
    ;	init
    #include<p18f4520.inc>
    ; == config == ;
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
    ; == variable == ;
	nH          EQU 0x20
	nL          EQU 0x21
	x0H         EQU 0x22
	x0L         EQU 0x23
	x1H         EQU 0x24
	x1L         EQU 0x25
    
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
    
    ADD16bits MACRO addEndH, addEndL, adderH, adderL
	MOVF adderL, W
	ADDWF addEndL, F
	MOVF adderH, W
	ADDWFC addEndH, F
    ENDM

    ; == main function == ;
    initData nH, 0x30
    initData nL, 0x21
    initData x0H, 0x26
    initData x0L, 0x5D
    
    RCALL newtonSqrt
    RCALL done
    
    newtonSqrt:
	MOVFF x0H, x1H
	MOVFF x0L, x1L
	MOVFF nH, DividendH
	MOVFF nL, DividendL
	newtonSqrtLoop:
	    MOVFF x1H, x0H
	    MOVFF x1L, x0L
	    
	    MOVFF x0H, DivisorH
	    MOVFF x0L, DivisorL
	    RCALL division
	    
	    MOVFF QuotientH, x1H
	    MOVFF QuotientL, x1L
	    
	    ; == x1 = x0 + x1 == ;
	    ADD16bits x1H, x1L, x0H, x0L
	    
	    ; == /2 == ;
	    RR16bits x1H, x1L
	    BCF x1H, d'7'

	    ; end the loop if x0 == x1
	    MOVF x1H, W
	    CPFSEQ x0H ; skip if x0H = x1H
	    BRA newtonSqrtLoop
	    MOVF x1L, W
	    CPFSEQ x0L ; skip if x0L = x1L
	    BRA newtonSqrtLoop
    RETURN
    
    
    division:
	; == init setting == ;
	initData count, 0x01
	CLRF QuotientH
	CLRF QuotientL
	MOVFF DividendH, RemainderH
	MOVFF DividendL, RemainderL
	
	; == preprocess divisor == ;
	; rotate Divisor to the left ; e.g. 0000 0001 0110 0111 -> 1011 0011 1000 000
	; store count as loop num
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