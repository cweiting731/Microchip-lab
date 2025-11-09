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
    
; ckeck button
check_process:
   BTFSS PORTB, 0
   BRA lightup
   BRA check_process

lightup:
    initData LATA, b'00000001'
    RCALL Delay0p75s
    initData LATA, b'00000010'
    RCALL Delay0p75s
    initData LATA, b'00000011'
    RCALL Delay0p75s
    initData LATA, b'00000101'
    RCALL Delay0p75s   
    CLRF LATA
    BRA check_process
    

; instruction time = 4 us
; delay 0.75s
; (2 + (2 + 8 * L1 + 3) * L2)
Delay0p75s:
    ; 2 cycles
    MOVLW   d'97'
    MOVWF   L2

Delay_outer2:
    ; 2 cycles
    MOVLW   d'241'
    MOVWF   L1

Delay_inner:
    ; 8 cycles
    NOP
    NOP
    NOP
    NOP
    NOP
    DECFSZ  L1, F
    BRA     Delay_inner
    
    ; 3 cycles
    DECFSZ  L2, F
    BRA     Delay_outer2

    RETURN
end
    