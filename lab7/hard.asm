#include "p18f4520.inc"

; CONFIG1H
  CONFIG  OSC = INTIO67         ; Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
  CONFIG  FCMEN = OFF           ; Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
  CONFIG  IESO = OFF            ; Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

; CONFIG2L
  CONFIG  PWRT = OFF            ; Power-up Timer Enable bit (PWRT disabled)
  CONFIG  BOREN = SBORDIS       ; Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
  CONFIG  BORV = 3              ; Brown Out Reset Voltage bits (Minimum setting)

; CONFIG2H
  CONFIG  WDT = OFF             ; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
  CONFIG  WDTPS = 32768         ; Watchdog Timer Postscale Select bits (1:32768)

; CONFIG3H
  CONFIG  CCP2MX = PORTC        ; CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
  CONFIG  PBADEN = ON           ; PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
  CONFIG  LPT1OSC = OFF         ; Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
  CONFIG  MCLRE = ON            ; MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

; CONFIG4L
  CONFIG  STVREN = ON           ; Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
  CONFIG  LVP = OFF             ; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
  CONFIG  XINST = OFF           ; Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

; CONFIG5L
  CONFIG  CP0 = OFF             ; Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
  CONFIG  CP1 = OFF             ; Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
  CONFIG  CP2 = OFF             ; Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
  CONFIG  CP3 = OFF             ; Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

; CONFIG5H
  CONFIG  CPB = OFF             ; Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
  CONFIG  CPD = OFF             ; Data EEPROM Code Protection bit (Data EEPROM not code-protected)

; CONFIG6L
  CONFIG  WRT0 = OFF            ; Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
  CONFIG  WRT1 = OFF            ; Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
  CONFIG  WRT2 = OFF            ; Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
  CONFIG  WRT3 = OFF            ; Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

; CONFIG6H
  CONFIG  WRTC = OFF            ; Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
  CONFIG  WRTB = OFF            ; Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
  CONFIG  WRTD = OFF            ; Data EEPROM Write Protection bit (Data EEPROM not write-protected)

; CONFIG7L
  CONFIG  EBTR0 = OFF           ; Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
  CONFIG  EBTR1 = OFF           ; Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
  CONFIG  EBTR2 = OFF           ; Table Read Protection bit (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
  CONFIG  EBTR3 = OFF           ; Table Read Protection bit (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

; CONFIG7H
  CONFIG  EBTRB = OFF           ; Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks)

    L1 EQU 0x14
    L2 EQU 0x15
    state EQU 0x16
    num EQU 0x17
    tmp1 EQU 0x18
    tmp2 EQU 0x19
 
    org 0x00
initData MACRO position, value
    MOVLW value
    MOVWF position
ENDM
delay MACRO num1, num2
    initData L1, num1
    initData L2, num2
    RCALL Delay
ENDM
	
; 程式邏輯：會一直卡在main裡面做無限迴圈，按下RB0的按鈕後會觸發interrupt，跳到ISR執行
; ISR裡的內容會亮起所有在RA上的燈泡，Delay約0.5秒後熄滅。

goto Initial			; 避免程式一開始就會執行到ISR這一段，要跳過。
    
org 0x08
HighISR:        ; RB0 interrupt (高優先)
    BTFSS INTCON, INT0IF
    RETFIE FAST
    RCALL buttonHandler
    BCF INTCON, INT0IF
    RETFIE FAST

org 0x18
LowISR:         ; Timer2 interrupt (低優先)
    BTFSS PIR1, TMR2IF
    RETFIE FAST
    ; 處理 Timer2
    INCF num
    INCF num
    
    SWAPF num, w
    MOVWF LATD
    
    BCF PIR1, TMR2IF
    RETFIE FAST                   ; 離開ISR，回到原本程式執行的位址，同時會將GIE設為1，允許之後的interrupt能夠觸發
    
    
Initial:				; 初始化的相關設定
    ; ==== 設定I/O ====
    MOVLW 0x0F
    MOVWF ADCON1            ; 將PORTB設定為數位輸入 (避免干擾)
    CLRF TRISD              ; RD7-RD4 為輸出 (LED)
    CLRF LATD               ; 先全部關掉LED
    BSF TRISB, 0            ; RB0 為輸入（按鈕）
    BCF INTCON2, INTEDG0    ; 設為「下降沿」觸發按鈕（按下觸發）

    ; ==== 清狀態變數 ====
    CLRF state              ; 0 = 0.5s ; 1 = 1s
    MOVLW 0x00
    MOVWF num               ; 初始顯示值（例如全亮）

    ; ==== 啟用中斷優先權 ====
    BSF RCON, IPEN          ; 啟用高低優先權系統

    ; ==== INT0（按鈕）設定 ====
    BCF INTCON, INT0IF      ; 清除INT0旗標
    BSF INTCON, INT0IE      ; 啟用INT0中斷
    BSF INTCON, GIEH        ; 啟用高優先權中斷（必要）

    ; ==== Timer2 設定 ====
    MOVLW b'01111111'       ; Postscale=1:16, Prescale=1:16, Timer2 OFF
    MOVWF T2CON
    MOVLW D'61'            ; 0.5 秒初始週期
    MOVWF PR2
    CLRF TMR2               ; 清除Timer2暫存
    BCF PIR1, TMR2IF        ; 清除旗標
    BSF PIE1, TMR2IE        ; 啟用Timer2中斷
    BCF IPR1, TMR2IP        ; 設為低優先權中斷
    BSF T2CON, TMR2ON       ; 啟動Timer2

    ; ==== 啟用全域中斷 ====
    BSF INTCON, GIEL        ; 啟用低優先權中斷
    BSF OSCCON, 0x20        ; 設定系統時脈為250kHz（根據你的原設定）
main:
    ; state 1 = 0000 0000 = even
    ; state 2 = 1111 1111 = odd
    BRA main
    
; instruction time = 4 us
; delay 0.5s (173, 90)
; delay 0.25s (31, 247)
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
    
buttonHandler:
    ; 處理按鈕
    BTFSC state, 0 ; skip if 0
    BRA odd2even ; odd -> even
    ; even -> odd
    even2odd:
	SETF state
	CLRF num
	INCF num
	initData PR2, d'244'
	BRA skip
    odd2even:
	CLRF state
	CLRF num
	initData PR2, d'61'
    skip:
	delay d'31', d'247'
RETURN
end






