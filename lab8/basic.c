// === Configuration Bits ===
#pragma config OSC = INTIO67
#pragma config PWRT = OFF
#pragma config BOREN = ON
#pragma config WDT = OFF
#pragma config PBADEN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF

#include <xc.h>
#include <pic18f4520.h>
#define _XTAL_FREQ 125000UL   // 用於 __delay_ms()

// === 函式宣告 ===
void setServoAngle(unsigned char state);

// === 主程式 ===
void main(void) {
    // === 時脈設定 ===
    OSCCONbits.IRCF = 0b001; // Fosc = 125kHz, Tosc = 8µs

    // === CCP1 (PWM) 設定 ===
    TRISCbits.TRISC2 = 0;    // RC2 (CCP1) 為輸出
    T2CONbits.T2CKPS = 0b01; // Timer2 預分頻 1:4
    PR2 = 0x9B;              // PWM 週期 ≈ 20ms (50Hz)
    CCP1CONbits.CCP1M = 0b1100; // PWM 模式
    T2CONbits.TMR2ON = 1;    // 啟動 Timer2

    // === 按鈕設定 ===
    TRISBbits.TRISB0 = 1;    // RB0 為輸入
//    INTCON2bits.RBPU = 0;    // 啟用 PORTB 上拉電阻
//    WPUBbits.WPUB0 = 1;      // RB0 上拉電阻打開

    // === 初始狀態 ===
    unsigned char state = 0; // 0: 0°, 1: +90°, 2: 0°, 3: -90°, 4: 0° ...
    setServoAngle(0);

    // === 主迴圈 ===
    while (1) {
        if (PORTBbits.RB0 == 0) {  // 按下按鈕
            __delay_ms(20);        // 消抖延遲
            while (PORTBbits.RB0 == 0); // 等待放開按鍵
            state++;
            if (state > 3) state = 0; // 循環 0~3
            setServoAngle(state);     // 更新角度

            __delay_ms(20);            // 放開後消抖
        }
    }
}

// === 伺服角度控制函式 ===
// 0 → 0°   (1.5ms)
// 1 → +90° (2.0ms)
// 2 → 0°   (1.5ms)
// 3 → -90° (1.0ms)
void setServoAngle(unsigned char state) {
    switch (state) {
        case 0: // 0°
        case 2:
            CCPR1L = 0x0B;        // 約 1.5ms
            CCP1CONbits.DC1B = 0b01;
            break;
        case 1: // +90°
            CCPR1L = 0x12;        // 約 2.0ms
            CCP1CONbits.DC1B = 0b11;
            break;
        case 3: // -90°
            CCPR1L = 0x03;        // 約 1.0ms
            CCP1CONbits.DC1B = 0b11;
            break;
    }
}
