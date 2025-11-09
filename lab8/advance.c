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
#define _XTAL_FREQ 125000UL   // 4 MHz internal clock

void setServoAngle(unsigned char angle);

void main(void) {
    // === 時脈設定 ===
    OSCCONbits.IRCF = 0b001; // Fosc = 125k Hz

    // === CCP1 (PWM) ===
    TRISCbits.TRISC2 = 0;
    T2CONbits.TMR2ON = 0;      // 關掉 Timer2 以設定
    T2CONbits.T2CKPS = 0b01;   // 1:4
    PR2 = 0x9B;                // ≈ 20ms period
    CCP1CONbits.CCP1M = 0b1100;// PWM mode
    T2CONbits.TMR2ON = 1;      // 啟動 Timer2

    // === 按鈕 ===
    TRISBbits.TRISB0 = 1;

    unsigned char ccpr = 15; // 15 ~ 75 -> -90 ~ 90
    unsigned char sign = 0; // 0 for + // 1 for -
    setServoAngle(ccpr);

    while (1) {
        if (PORTBbits.RB0 == 0) {
            __delay_ms(20);
            while (PORTBbits.RB0 == 0);
            for (int i=0; i<45; i++) {
                if (ccpr >= 75) sign = 0;
                if (ccpr <= 15) sign = 1;
                if (sign) ccpr++;
                else ccpr--;
                setServoAngle(ccpr);
                __delay_ms(15);
            }
            __delay_ms(20);
        }
    }
}

void setServoAngle(unsigned char ccpr) {
    if (ccpr >= 75) ccpr = 75;
    if (ccpr <= 15) ccpr = 15;
    CCPR1L = ccpr >> 2;
    CCP1CONbits.DC1B = ccpr & 0b11;
}
