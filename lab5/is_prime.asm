#include "xc.inc"
GLOBAL _is_prime
PSECT mytext, local, class=CODE, reloc=2
 ; == variable == ;
 testcast   EQU 0x10 ; testcast = 1 ~ 255
 count      EQU 0x11
 remainder  EQU 0x12
 dividend   EQU 0x13
 divisor    EQU 0x14
 divCount   EQU 0x15
 quotient   EQU 0x16
 
    
initData MACRO position, value
    MOVLW value
    MOVWF position
ENDM
    
_is_prime:
    MOVWF testcast
    
    MOVLW 0x01
    CPFSGT testcast ; skip if testcast > 1
    BRA not_prime ; testcast == 1
    
    initData count, 0x02
    
    prime_loop:
	; count * count
	MOVF count, W
	MULWF count
	TSTFSZ PRODH ; skip if PRODH is 0
	BRA is_prime ; the squre is 16 * 16 = 256 (0000 0001 0000 0000) ; PRODH > 0
	MOVF testcast, W
	CPFSGT PRODL ; skip if PRODL > testcast
	BRA div_check
	BRA is_prime
	
	div_check:
	    MOVFF testcast, dividend
	    MOVFF count, divisor
	    RCALL division

	    TSTFSZ remainder ; skip if remainder == 0
	    BRA primeContinue
	    BRA not_prime
	
	primeContinue:
	    INCF count
	    BRA prime_loop
	
    not_prime:
	MOVLW 0xFF
	BRA done
    is_prime:
	MOVLW 0x01
    done:
	RETURN
	
division:
    initData divCount, 0x08
    CLRF remainder
    CLRF quotient
    divLoop:
	RLCF dividend
	RLCF remainder
	RLNCF quotient, F
	
	MOVF divisor, W
	CPFSLT remainder ; skip if remainder < divisor
	BRA sub
	BRA divContinue
	
	sub:
	    SUBWF remainder, F
	    BSF quotient, 0
	    
	divContinue:
	    DECFSZ divCount ; skip if 0
	    BRA divLoop
    RETURN
    

;int isPrime(int n) { // ??????????????
;    // ??n????1?????0???1????????????
;    if (n <= 1) {
;        return 0;
;    }
;    // ?2????????i???????n?????n???sqrt(n)????????????sqrt(n)???
;    for (int i = 2; i * i <= n; i++) {
;        // ??n??i?????n???????0
;        if (n % i == 0) {
;            return 0;
;        }
;    }
;    // ????????????????n??????n??????1
;    return 1;
