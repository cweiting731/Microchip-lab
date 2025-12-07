#ifndef TIMERS_H
#define TIMERS_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * priority 參數說明：
 *   0 = 低優先權 (low priority)
 *   1 = 高優先權 (high priority)
 *
 * 注意：要使用高/低優先權中斷，必須先開啟：
 *   RCONbits.IPEN = 1;                  // 啟用中斷優先權功能
 *   INTCONbits.GIEH = 1;                // 啟用高優先權中斷
 *   INTCONbits.GIEL = 1;                // 啟用低優先權中斷
 * 或用你 lib.h 提供的：
 *   enableInterruptPriorityMode(1);
 *   enableGlobalInterrupt(1);
 *   enablePeripheralInterrupt(1);
 *
 * 各 Timer 參數說明：
 *
 * Timer0_Initialize(preload, prescaler, priority)
 *   preload   : TMR0 預載值（0~65535）
 *   prescaler : {1,2,4,8,16,32,64,128,256}
 *
 * Timer1_Initialize(preload, prescaler, priority)
 *   prescaler : {1,2,4,8}
 *
 * Timer2_Initialize(pr2, prescaler, postscale, priority)
 *   pr2       : 0~255
 *   prescaler : {1,4,16}
 *   postscale : {1~16}
 *
 * Timer3_Initialize(preload, prescaler, priority)
 *   prescaler : {1,2,4,8}
 */

void Timer0_Initialize(unsigned int preload,
                       unsigned int prescaler,
                       unsigned int priority);

void Timer1_Initialize(unsigned int preload,
                       unsigned int prescaler,
                       unsigned int priority);

void Timer2_Initialize(unsigned int pr2,
                       unsigned int prescaler,
                       unsigned int postscale,
                       unsigned int priority);

void Timer3_Initialize(unsigned int preload,
                       unsigned int prescaler,
                       unsigned int priority);

void Timer0_start();
void Timer0_stop();
void Timer0_reload(unsigned int preload);
void Timer0_clearInterruptFlag();
bool Timer0_isInterruptFlagSet();

void Timer1_start();
void Timer1_stop();
void Timer1_reload(unsigned int preload);
void Timer1_clearInterruptFlag();
bool Timer1_isInterruptFlagSet();

void Timer2_start();
void Timer2_stop();
void Timer2_reload(unsigned int pr2);
void Timer2_clearInterruptFlag();
bool Timer2_isInterruptFlagSet();

void Timer3_start();
void Timer3_stop();
void Timer3_reload(unsigned int preload);
void Timer3_clearInterruptFlag();
bool Timer3_isInterruptFlagSet();

#endif
