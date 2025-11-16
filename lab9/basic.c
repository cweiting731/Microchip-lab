#include <xc.h>
#include <pic18f4520.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config PBADEN = OFF
#pragma config LVP = OFF

#define THRESHOLD 32

// ===== 日期 8 個 digits，改成今天的日期 =====
const unsigned char date[8] = {2, 0, 2, 5, 0, 1, 0, 1};
unsigned char index = 8;

unsigned int preADC = 0;

// 顯示數字的 4bit binary
void displayDigit(unsigned char v)
{
    LATCbits.LATC0 = (v & 0x01);
    LATCbits.LATC1 = (v & 0x02) >> 1;
    LATCbits.LATC2 = (v & 0x04) >> 2;
    LATCbits.LATC3 = (v & 0x08) >> 3;
}

void __interrupt(high_priority) H_ISR()
{
    unsigned int adcValue = ((unsigned int)ADRESH << 8) | ADRESL;

    if (index == 8) { // 尚未初始化
        index = 0;
        preADC = adcValue;
        PIR1bits.ADIF = 0;
        ADCON0bits.GO = 1;
        return;
    }

    // 只有超過 THRESHOLD 才切換
    if (adcValue > preADC + THRESHOLD || adcValue < preADC - THRESHOLD)
    {
        index = (index + 1) % 8;
        displayDigit(date[index]);
        preADC = adcValue;
    }

    PIR1bits.ADIF = 0;
    ADCON0bits.GO = 1;
}

void main()
{
    OSCCONbits.IRCF = 0b100;  // 1 MHz

    // LED output
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC3 = 0;

    // 初始顯示第一個數字
    displayDigit(date[0]);

    // ADC 設定
    TRISAbits.RA0 = 1;
    ADCON1bits.PCFG = 0b1110; 
    ADCON0bits.CHS = 0;

    ADCON2bits.ADCS = 0b000;  
    ADCON2bits.ACQT = 0b001;  
    ADCON2bits.ADFM = 1;      
    ADCON0bits.ADON = 1;

    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;

    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    ADCON0bits.GO = 1;

    while(1);
}
