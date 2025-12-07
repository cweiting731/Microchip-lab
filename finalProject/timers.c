#include <xc.h>
#include "timers.h"
#include <stdint.h>
#include <stdbool.h>

/*
===========================================================
                       TIMER0
===========================================================
寄存器：T0CON, TMR0H, TMR0L, INTCON, INTCON2

優先權相關位元：
    INTCONbits.TMR0IE  : Timer0 中斷允許 (1=啟用, 0=關閉)
    INTCONbits.TMR0IF  : Timer0 中斷旗標 (1=發生, 0=未發生)
    INTCON2bits.TMR0IP : Timer0 中斷優先權 (1=高, 0=低)

T0CONbits.TMR0ON = 0/1
    0 = 關閉 Timer0
    1 = 啟動 Timer0

T0CONbits.T08BIT = 0/1
    0 = 16-bit mode (常用)
    1 = 8-bit mode

T0CONbits.T0CS = 0/1
    0 = 內部時鐘 Fosc/4
    1 = 外部腳位 T0CKI

T0CONbits.PSA = 0/1
    0 = 使用 prescaler
    1 = 不使用 prescaler (分頻固定為 1:1)

T0CONbits.T0PS = 000~111（當 PSA=0 時才有效）
    000 = 1:2
    001 = 1:4
    010 = 1:8
    011 = 1:16
    100 = 1:32
    101 = 1:64
    110 = 1:128
    111 = 1:256

溢出時間公式：
    T = Prescaler × (65536 – preload) × Tosc
*/

void Timer0_Initialize(unsigned int preload,
                       unsigned int prescaler,
                       unsigned int priority)
{
    unsigned char t0ps_bits = 0;
    unsigned char use_prescaler = 1;

    // === Prescaler 對應表 ===
    switch (prescaler)
    {
        case 1:
            use_prescaler = 0;       // 不使用 prescaler (PSA = 1)
            break;
        case 2:   t0ps_bits = 0b000; break; // 1:2
        case 4:   t0ps_bits = 0b001; break; // 1:4
        case 8:   t0ps_bits = 0b010; break; // 1:8
        case 16:  t0ps_bits = 0b011; break; // 1:16
        case 32:  t0ps_bits = 0b100; break; // 1:32
        case 64:  t0ps_bits = 0b101; break; // 1:64
        case 128: t0ps_bits = 0b110; break; // 1:128
        case 256: t0ps_bits = 0b111; break; // 1:256
        default:
            // 非法值 → 預設 1:8
            t0ps_bits = 0b010;
            prescaler = 8;
            break;
    }

    // 關閉 Timer0 以防設定時在跑
    T0CONbits.TMR0ON = 0;

    // 設定 Timer0 模式：16-bit、內部時鐘、是否使用 prescaler
    T0CONbits.T08BIT = 0;                   // 16-bit mode
    T0CONbits.T0CS   = 0;                   // Fosc/4
    T0CONbits.PSA    = (use_prescaler ? 0 : 1);
    T0CONbits.T0PS   = t0ps_bits;           // 分頻設定

    // 預載值（設定溢出起點）
    TMR0H = preload >> 8;
    TMR0L = preload & 0xFF;

    // 清除中斷旗標
    INTCONbits.TMR0IF = 0;

    // 設定優先權：
    // priority = 0 → 低優先權
    // priority = 1 → 高優先權
    if (priority == 1)
        INTCON2bits.TMR0IP = 1;             // 高優先權
    else
        INTCON2bits.TMR0IP = 0;             // 低優先權

    // 啟用 Timer0 中斷
    INTCONbits.TMR0IE = 1;

    // 啟動 Timer0
    T0CONbits.TMR0ON = 1;
}
void Timer0_start() {
    T0CONbits.TMR0ON = 1;                   // 啟動 Timer0
}
void Timer0_stop() {
    T0CONbits.TMR0ON = 0;                   // 關閉 Timer0
}
void Timer0_reload(unsigned int preload) {
    TMR0H = preload >> 8;
    TMR0L = preload & 0xFF;
}
void Timer0_clearInterruptFlag() {
    INTCONbits.TMR0IF = 0;                  // 清中斷旗標
}
bool Timer0_isInterruptFlagSet() {
    return INTCONbits.TMR0IF;
}


/*
===========================================================
                       TIMER1
===========================================================
寄存器：T1CON, TMR1H, TMR1L, PIR1, PIE1, IPR1

優先權相關位元：
    PIE1bits.TMR1IE : Timer1 中斷允許
    PIR1bits.TMR1IF : Timer1 中斷旗標
    IPR1bits.TMR1IP : Timer1 中斷優先權 (1=高, 0=低)

T1CONbits.RD16 = 0/1
    0 = 8-bit 存取
    1 = 16-bit 同步讀寫（建議）

T1CONbits.TMR1CS = 0/1
    0 = 使用 Fosc/4
    1 = 使用外部時鐘（如 32.768kHz 晶振）

T1CONbits.T1CKPS = 00/01/10/11 (Prescaler)
    00 = 1:1
    01 = 1:2
    10 = 1:4
    11 = 1:8

溢出時間公式：
    T = Prescaler × (65536 – preload) × Tosc
*/

void Timer1_Initialize(unsigned int preload,
                       unsigned int prescaler,
                       unsigned int priority)
{
    unsigned char ps_bits = 0;

    // Prescaler 對應表
    switch (prescaler)
    {
        case 1: ps_bits = 0b00; break;  // 1:1
        case 2: ps_bits = 0b01; break;  // 1:2
        case 4: ps_bits = 0b10; break;  // 1:4
        case 8: ps_bits = 0b11; break;  // 1:8
        default:
            ps_bits = 0b11;             // 預設 1:8
            prescaler = 8;
            break;
    }

    T1CONbits.TMR1ON = 0;               // 關閉 Timer1
    T1CONbits.RD16   = 1;               // 16-bit
    T1CONbits.TMR1CS = 0;               // 使用 Fosc/4
    T1CONbits.T1CKPS = ps_bits;         // 設定 Prescaler

    TMR1H = preload >> 8;
    TMR1L = preload & 0xFF;

    PIR1bits.TMR1IF = 0;                // 清中斷旗標

    // 設定中斷優先權
    if (priority == 1)
        IPR1bits.TMR1IP = 1;            // 高優先權
    else
        IPR1bits.TMR1IP = 0;            // 低優先權

    PIE1bits.TMR1IE = 1;                // 啟用 Timer1 中斷
    T1CONbits.TMR1ON = 1;               // 啟動 Timer1
}

void Timer1_start() {
    T1CONbits.TMR1ON = 1;               // 啟動 Timer1
}
void Timer1_stop() {
    T1CONbits.TMR1ON = 0;               // 關閉 Timer1
}
void Timer1_reload(unsigned int preload) {
    TMR1H = preload >> 8;
    TMR1L = preload & 0xFF;
}
void Timer1_clearInterruptFlag() {
    PIR1bits.TMR1IF = 0;                // 清中斷旗標
}
bool Timer1_isInterruptFlagSet() {
    return PIR1bits.TMR1IF;
}

/*
===========================================================
                       TIMER2
===========================================================
寄存器：T2CON, PR2, TMR2, PIR1, PIE1, IPR1

優先權相關位元：
    PIE1bits.TMR2IE : Timer2 中斷允許
    PIR1bits.TMR2IF : Timer2 中斷旗標
    IPR1bits.TMR2IP : Timer2 中斷優先權 (1=高, 0=低)

T2CONbits.T2CKPS = Prescaler
    00 = 1:1
    01 = 1:4
    1x = 1:16

T2CONbits.T2OUTPS = Postscaler (0000~1111)
    0000 = 1:1
    0001 = 1:2
    ...
    1111 = 1:16

週期公式：
    T = (PR2 + 1) × Prescaler × Postscaler × Tosc
*/

void Timer2_Initialize(unsigned int pr2,
                       unsigned int prescaler,
                       unsigned int postscale,
                       unsigned int priority)
{
    unsigned char ps_bits = 0;
    unsigned char post_bits = 0;

    // Prescaler 對應表
    switch (prescaler)
    {
        case 1:  ps_bits = 0b00; break; // 1:1
        case 4:  ps_bits = 0b01; break; // 1:4
        case 16: ps_bits = 0b10; break; // 1:16
        default:
            ps_bits = 0b01;             // 預設 1:4
            prescaler = 4;
            break;
    }

    // Postscaler：允許 1~16，對應到 0~15
    if (postscale >= 1 && postscale <= 16)
        post_bits = (unsigned char)(postscale - 1);
    else
        post_bits = 0;                  // 非法值 → 預設 1:1

    T2CONbits.TMR2ON = 0;               // 關閉 Timer2

    T2CONbits.T2CKPS  = ps_bits;        // 設定 Prescaler
    T2CONbits.T2OUTPS = post_bits;      // 設定 Postscaler

    PR2  = (unsigned char)pr2;          // 設定週期
    TMR2 = 0;                           // 清計數器

    PIR1bits.TMR2IF = 0;                // 清中斷旗標

    // 設定優先權
    if (priority == 1)
        IPR1bits.TMR2IP = 1;            // 高優先權
    else
        IPR1bits.TMR2IP = 0;            // 低優先權

    PIE1bits.TMR2IE = 1;                // 啟用中斷
    T2CONbits.TMR2ON = 1;               // 啟動 Timer2
}
void Timer2_start() {
    T2CONbits.TMR2ON = 1;               // 啟動 Timer2
}
void Timer2_stop() {
    T2CONbits.TMR2ON = 0;               // 關閉 Timer2
}
void Timer2_reload(unsigned int pr2) {
    PR2  = (unsigned char)pr2;          // 設定週期
    TMR2 = 0;                           // 清計數器
}
void Timer2_clearInterruptFlag() {
    PIR1bits.TMR2IF = 0;                // 清中斷旗標
}
bool Timer2_isInterruptFlagSet() {
    return PIR1bits.TMR2IF;
}


/*
===========================================================
                       TIMER3
===========================================================
寄存器：T3CON, TMR3H, TMR3L, PIR2, PIE2, IPR2

優先權相關位元：
    PIE2bits.TMR3IE : Timer3 中斷允許
    PIR2bits.TMR3IF : Timer3 中斷旗標
    IPR2bits.TMR3IP : Timer3 中斷優先權 (1=高, 0=低)

T3CONbits.T3CKPS = Prescaler
    00 = 1:1
    01 = 1:2
    10 = 1:4
    11 = 1:8

T3CONbits.TMR3CS = 0/1
    0 = Fosc/4
    1 = 外部時鐘
*/

void Timer3_Initialize(unsigned int preload,
                       unsigned int prescaler,
                       unsigned int priority)
{
    unsigned char ps_bits = 0;

    // Prescaler 對應
    switch (prescaler)
    {
        case 1: ps_bits = 0b00; break;
        case 2: ps_bits = 0b01; break;
        case 4: ps_bits = 0b10; break;
        case 8: ps_bits = 0b11; break;
        default:
            ps_bits = 0b11;             // 預設 1:8
            prescaler = 8;
            break;
    }

    T3CONbits.TMR3ON = 0;               // 關閉 Timer3
    T3CONbits.RD16   = 1;               // 16-bit mode
    T3CONbits.TMR3CS = 0;               // 使用 Fosc/4
    T3CONbits.T3CKPS = ps_bits;

    TMR3H = preload >> 8;
    TMR3L = preload & 0xFF;

    PIR2bits.TMR3IF = 0;                // 清中斷旗標

    // 設定優先權
    if (priority == 1)
        IPR2bits.TMR3IP = 1;            // 高優先權
    else
        IPR2bits.TMR3IP = 0;            // 低優先權

    PIE2bits.TMR3IE = 1;                // 啟用 Timer3 中斷
    T3CONbits.TMR3ON = 1;               // 啟動 Timer3
}
void Timer3_start() {
    T3CONbits.TMR3ON = 1;               // 啟動 Timer3
}
void Timer3_stop() {
    T3CONbits.TMR3ON = 0;               // 關閉 Timer3
}
void Timer3_reload(unsigned int preload) {
    TMR3H = preload >> 8;
    TMR3L = preload & 0xFF;
}
void Timer3_clearInterruptFlag() {
    PIR2bits.TMR3IF = 0;                // 清中斷旗標
}
bool Timer3_isInterruptFlagSet() {
    return PIR2bits.TMR3IF;
}