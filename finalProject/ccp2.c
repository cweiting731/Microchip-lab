#include <xc.h>
#include <stdint.h>
#include "ccp2.h"
#include "config.h"

/**
 * @brief 依據 prescale 值取得 T2CKPS bits
 *        （如果 CCP1 那邊已經有一個一樣的函式，這個可以改成共用的）
 */
static uint8_t CCP2_GetPrescalerBits(uint8_t prescale)
{
    switch (prescale)
    {
        case 1:  return 0b00;
        case 4:  return 0b01;
        case 16: return 0b10;
        default: return 0b00;  // 預設 1:1
    }
}

void CCP2_PWM_Initialize(uint32_t frequency, uint8_t prescale)
{
    // ========= 計算 PR2 =========
    //
    // PWM_freq = Fosc / (4 * prescale * (PR2+1))
    // → PR2 = (Fosc / (4*prescale*freq)) - 1
    //
    uint32_t pr2_value =
        (_XTAL_FREQ / (4UL * (uint32_t)prescale * frequency)) - 1;

    if (pr2_value > 255)
        pr2_value = 255;   // PR2 只有 8 bit

    // ========= 設定 CCP2 為 PWM 模式 =========
    CCP2CONbits.CCP2M = 0b1100;  // PWM mode

    // ========= 設定 TMR2 =========
    // 注意：如果 CCP1 已經在用 TMR2 做 PWM，
    //       請確保 frequency 與 prescale 設定完全相同。
    T2CONbits.TMR2ON = 0;               // 關掉 TMR2 才能設 PR2
    T2CONbits.T2CKPS = CCP2_GetPrescalerBits(prescale);

    PR2 = (uint8_t)pr2_value;           // 設定 PWM period

    // ========= Duty 初值 = 0 =========
    CCPR2L = 0;
    CCP2CONbits.DC2B = 0;

    // ========= 開啟 TMR2 =========
    PIR1bits.TMR2IF = 0;
    T2CONbits.TMR2ON = 1;

    // ========= 設定 CCP2 腳為輸出 =========
    // CCP2MX = PORTC 時，CCP2 腳在 RC1
    TRISCbits.TRISC1 = 0;  // CCP2 pin = RC1

    // ========= CCP2 啟動 PWM =========
    // 現在 TMR2 開始跑 → PWM 開始輸出
}

void CCP2_SetDuty(uint16_t duty)
{
    if (duty > 1023)
        duty = 1023;  // Max 10-bit

    // CCPR2L = duty[9:2]
    CCPR2L = (uint8_t)(duty >> 2);

    // DC2B = duty[1:0]
    CCP2CONbits.DC2B = duty & 0x03;
}

/**
 * @brief 將角度(0~180°)轉成 PWM Duty(0~1023) — 給 CCP2 用
 *
 *   - 0°   = 450 µs
 *   - 180° = 2450 µs
 *   - PWM Period = 20000 µs (50Hz)
 */
void Servo2_WriteAngle(uint16_t angle, uint8_t prescale)
{
    if (angle > 180) angle = 180;
    // angle 是 uint16_t，不可能 < 0，這行可省略：
    // if (angle < 0) angle = 0;

    // ===== 1. 角度 → 脈寬(µs) =====
    uint32_t pulse_us =
        ((uint32_t)angle * (2450 - 450)) / 180 + 450;

    // ===== 2. 脈寬 → CCP duty (10-bit) =====
    //
    // duty = pulse_us / (Tosc * prescale)
    float Tosc_us = (1.0f / (float)_XTAL_FREQ) * 1000000.0f; // µs
    float rawCCPR = pulse_us / (Tosc_us * (float)prescale);

    uint16_t duty = (uint16_t)(rawCCPR);  // 轉成整數

    if (duty > 1023) duty = 1023;

    CCP2_SetDuty(duty);
}

void Servo2_WritePulseUS(uint16_t pulse_us, uint8_t prescale)
{
    // 限制安全範圍（1000 ~ 2000 us，可依實際伺服馬達調）
    if (pulse_us < 1000) pulse_us = 1000;
    if (pulse_us > 2000) pulse_us = 2000;

    float Tosc_us = (1.0f / (float)_XTAL_FREQ) * 1000000.0f;

    // duty = pulse_us / (Tosc * prescale)
    float raw = pulse_us / (Tosc_us * (float)prescale);

    uint16_t duty = (uint16_t)(raw);
    if (duty > 1023) duty = 1023;

    CCP2_SetDuty(duty);
}
