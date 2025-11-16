#include <xc.h>
#include <pic18f4520.h>
#include <stdio.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config PWRT = OFF
#pragma config BOREN = ON
#pragma config PBADEN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF


unsigned char PreState = 8; // 0 ~ 7

void display4bitsOnCLED(unsigned char value) {
    if (PreState > value) { // decreasing -> odd
        LATCbits.LATC0 = 1;
    } else if (PreState < value) { // increasing -> even
        LATCbits.LATC0 = 0;
    }
    // 將value的值放進去 value = 0 ~ 7
    LATCbits.LATC1 = (value & 0x01);
    LATCbits.LATC2 = (value & 0x02) >> 1;
    LATCbits.LATC3 = (value & 0x04) >> 2;
    PreState = value;
}

void __interrupt(high_priority) H_ISR(){
    unsigned int adcValue;

    // read ADC result (left justified OFF → right justified)
    adcValue = ((unsigned int)ADRESH << 8) | ADRESL;  // 0~1023
    unsigned char state = (adcValue >> 7) & 0x07; // 0~7
    
    display4bitsOnCLED(state);
    
    PIR1bits.ADIF = 0;      // clear ADC interrupt flag
    ADCON0bits.GO = 1;      // start next conversion
}

void main(void){

    // ===== Clock =====
    OSCCONbits.IRCF = 0b100; // 1 MHz, 1 µs

    // ===== ADC 設定 (AN0) =====
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    TRISAbits.RA0 = 1;       // AN0 input
    ADCON1bits.PCFG = 0b1110; // only AN0 analog
    ADCON0bits.CHS  = 0b0000; // channel 0 = AN0

    ADCON2bits.ADCS = 0b000;  // Fosc/2, Tosc * 2 = 2 µs = Tad (> 0.7 µs)
    ADCON2bits.ACQT = 0b001;  // 2 Tad, Tacq = 4 µs (> 2.4 µs)
    ADCON2bits.ADFM = 1;      // right justified (建議用右對齊)
    ADCON0bits.ADON = 1;      // turn on ADC

    // ===== setting LED output =====
    TRISCbits.TRISC0 = 0;   // RC0 = output
    TRISCbits.TRISC1 = 0;   // RC1 = output
    TRISCbits.TRISC2 = 0;   // RC2 = output
    TRISCbits.TRISC3 = 0;   // RC3 = output

    // ===== ADC interrupt =====
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;

    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    // ===== Start ADC =====
    ADCON0bits.GO = 1;

    while(1);
}
