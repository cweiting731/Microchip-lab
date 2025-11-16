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

void __interrupt(high_priority) H_ISR(){
    unsigned int adcValue;

    // read ADC result (left justified OFF → right justified)
    adcValue = ((unsigned int)ADRESH << 8) | ADRESL;  // 0~1023

    // scale to PWM duty (0~1023 → 0~1023 for 10-bit PWM)
    CCPR1L  = adcValue >> 2;        // high 8 bits
    CCP1CONbits.DC1B = adcValue & 0x03;   // low 2 bits

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

    // ===== PWM 設定 (CCP1 = RC2) =====
    TRISCbits.TRISC2 = 0;   // RC2 = output

    // Set up PR2, CCP to decide PWM period and Duty Cycle
    /*
     * PWM period
     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
     * = (0x9B + 1) * 4 * 8µs * 4
     * = 0.019968s ~= 20ms
     */
    PR2 = 0xFF;             // PWM 頻率：1MHz / 4 / 4 / 256 ≈ 244 Hz
    CCP1CONbits.CCP1M = 0b1100; // PWM mode

    CCPR1L = 0;             // initial duty = 0
    T2CON = 0b00000101;     // Timer2 ON, prescale = 16

    // ===== ADC interrupt =====
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;

    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    // ===== Start ADC =====
    ADCON0bits.GO = 1;

    while(1);
}
