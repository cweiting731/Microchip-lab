#include <xc.h>
#include <stdint.h>
#include "adc.h"
#include <stdbool.h>

uint16_t preAdcValue = 0;

void ADC_Initialize(uint8_t analogMask,
                    uint8_t vrefPlus,
                    uint8_t vrefMinus,
                    uint8_t acqt,
                    uint8_t adcs,
                    uint8_t rightJustify)
{
    // ===== Voltage Reference =====
    ADCON1bits.VCFG0 = vrefPlus;   // 0=Vdd, 1=Vref+
    ADCON1bits.VCFG1 = vrefMinus;  // 0=Vss, 1=Vref-

    // ===== Analog or Digital Pins =====
    ADCON1bits.PCFG = analogMask;  // 0~15 編碼，決定哪個 ANx 是 analog

    // ===== Acquisition Time =====
    ADCON2bits.ACQT = acqt & 0b111;  // 取樣時間 = 2~20 Tad

    // ===== ADC Clock (ADCS) =====
    ADCON2bits.ADCS = adcs & 0b111;  // A/D clock source

    // ===== Result Justification =====
    ADCON2bits.ADFM = (rightJustify ? 1 : 0);

    // ===== Enable ADC module =====
    ADCON0bits.ADON = 1;
}

void ADC_enableInterrupt(void) {
    PIE1bits.ADIE = 1;  // Enable ADC interrupt
    PIR1bits.ADIF = 0;  // Clear ADC interrupt flag
    INTCONbits.PEIE = 1; // Enable peripheral interrupts
    INTCONbits.GIE = 1;  // Enable global interrupts
}

void ADC_disableInterrupt(void) {
    PIE1bits.ADIE = 0;  // Disable ADC interrupt
    PIR1bits.ADIF = 0;  // Clear ADC interrupt flag
}

bool ADC_isInterruptFlagSet(void) {
    return PIR1bits.ADIF;
}

void ADC_clearInterruptFlag(void) {
    PIR1bits.ADIF = 0;
}

void ADC_startInterruptRead(uint8_t channel) {
    // 防止 channel overflow（PIC18F4520 最多到 AN12）
    channel &= 0b1111;

    // ===== select channel =====
    ADCON0bits.CHS = channel;

    // __delay_us(5);         // acquisition delay (取樣時間)

    ADCON0bits.GO = 1;     // start conversion
}

void ADC_stopInterruptRead(void) {
    ADCON0bits.GO = 0;     // stop conversion
}

uint16_t ADC_Read(uint8_t channel)
{
    // 防止 channel overflow（PIC18F4520 最多到 AN12）
    channel &= 0b1111;

    // ===== select channel =====
    ADCON0bits.CHS = channel;

    // __delay_us(5);         // acquisition delay (取樣時間)

    ADCON0bits.GO = 1;     // start conversion
    while (ADCON0bits.GO); // wait until done

    uint16_t currentAdcValue = ((ADRESH << 8) | ADRESL);
    if (currentAdcValue > preAdcValue + THRESHOLD || currentAdcValue + THRESHOLD < preAdcValue) {
        preAdcValue = currentAdcValue;
    }

    return preAdcValue;
}

// void ADC_Initialize(void) {
//     TRISA = 0xff;		// Set as input port
//     ADCON1 = 0x0e;  	// Ref vtg is VDD & Configure pin as analog pin 
//     // ADCON2 = 0x92;  	
//     ADFM = 1 ;          // Right Justifie
//     ADCON2bits.ADCS = 7; // 
//     ADRESH=0;  			// Flush ADC output Register
//     ADRESL=0;  
// }

// int ADC_Read(int channel)
// {
//     int digital;
    
//     ADCON0bits.CHS =  0x07; // Select Channe7
//     ADCON0bits.GO = 1;
//     ADCON0bits.ADON = 1;
    
//     while(ADCON0bits.GO_nDONE==1);

//     return ((ADRESH << 8) | ADRESL);
// }