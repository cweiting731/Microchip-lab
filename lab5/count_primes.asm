#include "xc.inc"
GLOBAL _count_primes
PSECT mytext, local, class=CODE, reloc=2
 ; == variable == ;
 startH  EQU 0x21
 startL  EQU 0x22
 endH    EQU 0x23
 endL    EQU 0x24
 countH  EQU 0x25
 countL  EQU 0x26
 tmpH    EQU 0x27
 tmpL    EQU 0x28
    
 is_prime_count EQU 0x30
 dividendH EQU 0x31
 dividendL EQU 0x32
 divisorH EQU 0x33
 divisorL EQU 0x34
 remainderH EQU 0x35
 remainderL EQU 0x36
 divCount EQU 0x37
 quotientH EQU 0x38
 quotientL EQU 0x39

initData MACRO position, value
    MOVLW value
    MOVWF position
ENDM
    
INCF16bits MACRO positionH, positionL
    INCF positionL
    BTFSC STATUS, 0x02 ; skip if positionL != 0
    INCF positionH
ENDM
    
RL16bits MACRO rotateRH, rotateRL
    RLCF rotateRL
    RLCF rotateRH
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
    
MOVFF16bits MACRO srcH, srcL, desH, desL
    MOVFF srcH, desH
    MOVFF srcL, desL
ENDM
    
_count_primes:
    ; == init == ;
    BCF WDTCON, 0
    CLRF countH
    CLRF countL
    MOVFF 0x02, startH
    MOVFF 0x01, startL
    MOVFF 0x04, endH
    MOVFF 0x03, endL
    
    count_primes_loop:
	CLRF tmpH
	CLRF tmpL
	SUB16bits tmpH, tmpL, startH, startL
	BTFSC STATUS, 0x00
	BRA count_primes_done ; >= 0 ; == 0
	; < 0 ; > 0
    
	MOVFF endH, tmpH
	MOVFF endL, tmpL
	SUB16bits tmpH, tmpL, startH, startL ; end - start
	BTFSC STATUS, 0x00 ; skip if c is 0
	BRA count_primes_loop_keep ; >= 0
	BRA count_primes_done      ; < 0
	
	count_primes_loop_keep:
	    RCALL is_prime_16bits
	    TSTFSZ WREG ; skip if w == 0 (not prime)
	    BRA count_primes_increase      ; is prime
	    BRA count_primes_loop_continue ; not prime
	    
	    count_primes_increase:
		INCF16bits countH, countL
	    
	    count_primes_loop_continue:
		INCF16bits startH, startL
		BRA count_primes_loop
	    
	count_primes_done:
	    MOVFF countH, 0x02
	    MOVFF countL, 0x01
    
    RETURN

is_prime_16bits:
    initData tmpH, 0x00
    initData tmpL, 0x01
    SUB16bits tmpH, tmpL, startH, startL
    BTFSC STATUS, 0x00 ; skip if c is 0
    BRA not_prime ; == 1
    
    initData tmpH, 0x00
    initData tmpL, 0x02
    SUB16bits tmpH, tmpL, startH, startL
    BTFSC STATUS, 0x00 ; skip if c is 0
    BRA is_prime ; == 2
    
    BTFSS startL, 0x00
    BRA not_prime
    
    initData is_prime_count, 0x02 ; > 1
    prime_loop:
	MOVF is_prime_count, W
	BZ is_prime ; count == 256 ; 256^2 > 65535
	MULWF is_prime_count
	MOVFF16bits startH, startL, tmpH, tmpL
	SUB16bits tmpH, tmpL, PRODH, PRODL
	BTFSC STATUS, 0x00 ; skip if c is 0
	BRA div_check ; >= 0 ; i^2 <= n
	BRA is_prime  ; < 0 ; i^2 > n
	
	div_check:
	    MOVFF16bits startH, startL, dividendH, dividendL
	    initData divisorH, 0x00
	    MOVFF is_prime_count, divisorL
	    RCALL division_16bits
	    
	    CLRF tmpH
	    CLRF tmpL
	    SUB16bits tmpH, tmpL, remainderH, remainderL
	    BTFSC STATUS, 0x00
	    BRA not_prime ; >= 0 ; remainder == 0
	    ; < 0 ; remainder != 0
	    INCF is_prime_count
	    BRA prime_loop
	
    is_prime:
	MOVLW 0x01
	BRA done
    not_prime:
	MOVLW 0x00
    done:
RETURN
    
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