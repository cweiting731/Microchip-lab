/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef ADC_H
#define ADC_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#define THRESHOLD 4

/**
 * @brief 初始化 ADC 模組
 *
 * @param analogMask PCFG 設定（哪些腳為 analog）
 *        例：0b1110 → AN0 analog, AN1~AN12 digital
 *
 * @param vrefPlus 0=Vdd, 1=External Vref+
 * @param vrefMinus 0=Vss, 1=External Vref-
 *
 * @param acqt Acquisition Time (0–7)
 *        000 = 0TAD, 
 *        001 = 2TAD, 
 *        010 = 4TAD,
 *        011 = 6TAD,
 *        100 = 8TAD,
 *        101 = 12TAD,
 *        110 = 16TAD,
 *        111 = 20TAD
 *
 * @param adcs ADC Clock Select (0–7)
 *        111 = FRC (internal RC oscillator)
 *        110 = Fosc/64
 *        101 = Fosc/16
 *        100 = Fosc/4
 *        011 = FRC (internal RC oscillator)
 *        010 = Fosc/32
 *        001 = Fosc/8
 *        000 = Fosc/2
 *
 * @param rightJustify 1=右對齊, 0=左對齊
 */
void ADC_Initialize(uint8_t analogMask,
                    uint8_t vrefPlus,
                    uint8_t vrefMinus,
                    uint8_t acqt,
                    uint8_t adcs,
                    uint8_t rightJustify);

void ADC_enableInterrupt(void);
void ADC_disableInterrupt(void);
bool ADC_isInterruptFlagSet(void);
void ADC_clearInterruptFlag(void);
void ADC_startInterruptRead(uint8_t channel);
void ADC_stopInterruptRead(void);

/**
 * @brief 讀取 ADC 數值
 *
 * @param channel 要讀取的 AN 通道 (0~12)
 * @return uint16_t 10-bit ADC 數值 (0~1023)
 */
uint16_t ADC_Read(uint8_t channel);

#endif
