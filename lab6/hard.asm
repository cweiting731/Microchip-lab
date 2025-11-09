LIST p=18f4520
    #include<p18f4520.inc>
	CONFIG OSC = INTIO67 ; 1 MHZ
	CONFIG WDT = OFF
	CONFIG LVP = OFF
	
	L1	EQU 0x14
	L2	EQU 0x15
	L3      EQU 0x16
	tmp1    EQU 0x17
        tmp2    EQU 0x18
	org 0x00

start:
int:
; let pin can receive digital signal 
MOVLW 0x0f
MOVWF ADCON1
CLRF PORTB
BSF TRISB, 0
CLRF LATA
BCF TRISA, 0
BCF TRISA, 1
BCF TRISA, 2
    
initData MACRO position, value
    MOVLW value
    MOVWF position
ENDM
checkInput MACRO noInput, haveInput
    BTFSC PORTB, 0
    BRA noInput
    BRA haveInput
ENDM
delay MACRO num1, num2
    initData L1, num1
    initData L2, num2
    RCALL Delay
ENDM

state0:
    CLRF LATA
    delay d'31', d'247'
    BTFSS PORTB, 0
    BRA state1
    BRA state0
    
state1:
    initData LATA, b'00000001'
    delay d'31', d'247'
    BTFSS PORTB, 0
    BRA state2
    delay d'31', d'247'
    BTFSS PORTB, 0
    BRA state2
    
    initData LATA, b'00000010'
    delay d'31', d'247'
    BTFSS PORTB, 0
    BRA state2
    delay d'31', d'247'
    BTFSS PORTB, 0
    BRA state2
    
    initData LATA, b'00000100'
    delay d'31', d'247'
    BTFSS PORTB, 0
    BRA state2
    delay d'31', d'247'
    BTFSS PORTB, 0
    BRA state2
    BRA state1    
    
state2:
    ; state 2_1
    initData LATA, b'00000001'
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    
    ; state 2_2
    initData LATA, b'00000011'
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    
    ; state 2_3_1
    initData LATA, b'00000110'
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    
    ; state 2_3_2
    initData LATA, b'00000100'
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    
    ; state 2_3_3
    initData LATA, b'00000110'
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    
    ; state 2_3_4
    initData LATA, b'00000100'
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    
    ; state 2_3_5
    initData LATA, b'00000110'
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    delay d'31', d'247' ; 0.25s
    BTFSS PORTB, 0
    BRA state0
    BRA state2
    

; instruction time = 4 us
; delay 0.75s
; (2 + (2 + 8 * L1 + 3) * L2)
Delay:
    ; 2 cycles
    MOVF L2, W
    MOVWF tmp2

Delay_outer2:
    ; 2 cycles
    MOVF L1, W
    MOVWF tmp1

Delay_inner:
    ; 8 cycles
    NOP
    NOP
    NOP
    NOP
    NOP
    DECFSZ  tmp1, F
    BRA     Delay_inner
    
    ; 3 cycles
    DECFSZ  tmp2, F
    BRA     Delay_outer2

    RETURN
end
    
	    

    



