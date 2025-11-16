#include <xc.h>
#include <pic18f4520.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config PBADEN = OFF
#pragma config LVP = OFF

#define THRESHOLD 32   // 建議 5~10，越大越不抖動

unsigned int preADC = 0;

unsigned char num = 17;
unsigned char isIncreasing = 1; // 1: increasing, 0: decreasing

// ===== 顯示 4-bit 整數到 RC0~RC3 =====
void display4bits(unsigned char v)
{
    LATCbits.LATC0 = (v & 0x01);
    LATCbits.LATC1 = (v & 0x02) >> 1;
    LATCbits.LATC2 = (v & 0x04) >> 2;
    LATCbits.LATC3 = (v & 0x08) >> 3;
}

void __interrupt(high_priority) H_ISR()
{
    unsigned int adcValue = ((unsigned int)ADRESH << 8) | ADRESL;

    if (num == 17) { // 初始狀態
        num = 0;
        preADC = adcValue;
        display4bits(num);
        PIR1bits.ADIF = 0;
        ADCON0bits.GO = 1;
        return;
    }

    // ★★★ Threshold 判斷方向（完全不會亂跳） ★★★
    if (adcValue > preADC + THRESHOLD) // increasing
    {
        if (isIncreasing == 0) { // 剛切換
            // direction changed
            isIncreasing = 1;
            num = (num + 1) % 16;
            display4bits(num);
        } else {
            num = (num + 2) % 16;
            display4bits(num);
        }
        preADC = adcValue;
    }
    else if (adcValue < preADC - THRESHOLD) // decreasing
    {
        if (isIncreasing == 1) { // 剛切換
            // direction changed
            isIncreasing = 0;
            num = (num - 1 + 16) % 16;
            display4bits(num);
        } else {
            num = (num - 2 + 16) % 16;
            display4bits(num);
        }
        preADC = adcValue;
    }

    PIR1bits.ADIF = 0;
    ADCON0bits.GO = 1;
}

void main()
{
    // ===== Clock =====
    OSCCONbits.IRCF = 0b100;  // 1 MHz

    // ===== LED output =====
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC3 = 0;

    // ===== ADC 設定 =====
    TRISAbits.RA0 = 1;
    ADCON1bits.PCFG = 0b1110;  // AN0 analog
    
    ADCON0bits.CHS = 0;        // AN0
    
    ADCON2bits.ADCS = 0b000;   // Tad = 2µs
    ADCON2bits.ACQT = 0b001;   // Tacq = 4µs
    ADCON2bits.ADFM = 1;

    ADCON0bits.ADON = 1;

    // ===== Interrupt =====
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    ADCON0bits.GO = 1;

    while(1);
}
