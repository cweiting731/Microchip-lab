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

#define _XTAL_FREQ 125000UL   // 4 MHz internal clock (建議用 4MHz，不要125kHz)

// === 全域變數 ===
volatile unsigned char ccpr = 15;  // 15~75 對應 -90°~+90°
volatile unsigned char sign = 1;    // 1: 往右 (+)，0: 往左 (-)
volatile unsigned char running = 0;          // 0: 停止, 1: 運動

// === 函式宣告 ===
void setServoAngle(unsigned char ccpr);
void setupPWM(void);
void setupTimer1(void);

// === 中斷函式 ===
void __interrupt() ISR(void) {
    if (PIR1bits.TMR1IF) {       // Timer1 溢位中斷
        PIR1bits.TMR1IF = 0;     // 清除旗標
        TMR1H = 0xFF;            // 20ms reload
        TMR1L = 0x62;

        if (running) {
            if (ccpr >= 75) sign = 0;
            if (ccpr <= 15) sign = 1;
            if (sign) ccpr++;
            else ccpr--;

            setServoAngle(ccpr);

            // LED 亮度同步變化（使用簡單線性亮度）
            unsigned char brightness = (ccpr - 15) * 255 / (75 - 15); // 0~255
            CCPR2L = brightness >> 2; 
            CCP2CONbits.DC2B = brightness & 0b11;
        }
    }
}

// === 主程式 ===
void main(void) {
    // === 時脈設定 ===
    OSCCONbits.IRCF = 0b001;  // 4 MHz internal

    // === I/O ===
    TRISCbits.TRISC2 = 0;  // RC2 -> CCP1 (伺服)
    TRISCbits.TRISC1 = 0;  // RC1 -> CCP2 (LED)
    TRISBbits.TRISB0 = 1;  // RB0 -> 按鈕
    // INTCON2bits.RBPU = 0;  // 啟用上拉
    // WPUB = 0x01;

    setupPWM();
    setupTimer1();

    setServoAngle(ccpr);

    while (1) {
        if (PORTBbits.RB0 == 0) {     // 按下按鈕
            __delay_ms(20);           // 消抖
            if (PORTBbits.RB0 == 0) {
                running = !running;   // 切換運動狀態
                while (PORTBbits.RB0 == 0); // 等放開
                __delay_ms(20);
            }
        }
    }
}

// === Servo PWM ===
void setupPWM(void) {
    // CCP1 -> Servo
    PR2 = 0x9B;                // PWM period ≈ 20ms
    T2CONbits.T2CKPS = 0b01;   // 1:4 prescale
    CCP1CONbits.CCP1M = 0b1100;// PWM mode
    TRISCbits.TRISC2 = 0;
    T2CONbits.TMR2ON = 1;

    // CCP2 -> LED
    CCP2CONbits.CCP2M = 0b1100;
    TRISCbits.TRISC1 = 0;
    CCPR2L = 0;
}

// === Timer1 ===
void setupTimer1(void) {
    T1CON = 0b00110001;  // Timer1 on, 1:8 prescale
    TMR1H = 0xFF;        // 約20ms中斷
    TMR1L = 0x62;
    PIR1bits.TMR1IF = 0;
    PIE1bits.TMR1IE = 1; // 啟用中斷
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}

// === 設定伺服角度 ===
void setServoAngle(unsigned char ccprValue) {
    if (ccprValue >= 75) ccprValue = 75;
    if (ccprValue <= 15) ccprValue = 15;
    CCPR1L = ccprValue >> 2;
    CCP1CONbits.DC1B = ccprValue & 0b11;
}
