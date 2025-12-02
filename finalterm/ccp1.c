#include <xc.h>
#include <stdint.h>
#include "ccp1.h"
#include "config.h"

/**
 * @brief 依據 prescale 值取得 T2CKPS bits
 */
static uint8_t CCP1_GetPrescalerBits(uint8_t prescale)
{
    switch (prescale)
    {
        case 1:  return 0b00;
        case 4:  return 0b01;
        case 16: return 0b10;
        default: return 0b00;  // 預設 1:1
    }
}

void CCP1_PWM_Initialize(uint32_t frequency, uint8_t prescale)
{
    // ========= 計算 PR2 =========
    //
    // PWM_freq = Fosc / (4 * prescale * (PR2+1))
    //
    // → PR2 = (Fosc / (4*prescale*freq)) - 1
    //
    uint32_t pr2_value =
        (_XTAL_FREQ / (4UL * (uint32_t)prescale * frequency)) - 1;

    if (pr2_value > 255)
        pr2_value = 255;   // PR2 只有 8 bit

    // ========= 設定 CCP1 為 PWM 模式 =========
    CCP1CONbits.CCP1M = 0b1100;  // PWM mode

    // ========= 設定 TMR2 =========
    T2CONbits.TMR2ON = 0;               // 關掉 TMR2 才能設 PR2
    T2CONbits.T2CKPS = CCP1_GetPrescalerBits(prescale);

    PR2 = (uint8_t)pr2_value;           // 設定 PWM period

    // ========= Duty 初值 = 0 =========
    CCPR1L = 0;
    CCP1CONbits.DC1B = 0;

    // ========= 開啟 TMR2 =========
    PIR1bits.TMR2IF = 0;
    T2CONbits.TMR2ON = 1;

    // ========= 設定 CCP1 腳為輸出 =========
    TRISCbits.TRISC2 = 0;  // CCP1 pin = RC2

    // ========= CCP1 啟動 PWM =========
    // 現在 TMR2 開始跑 → PWM 開始輸出
}

void CCP1_SetDuty(uint16_t duty)
{
    if (duty > 1023)
        duty = 1023;  // Max 10-bit

    // CCPR1L = duty[9:2]
    CCPR1L = (uint8_t)(duty >> 2);

    // DC1B = duty[1:0]
    CCP1CONbits.DC1B = duty & 0x03;
}

/**
 * @brief 將角度(0~180°)轉成 PWM Duty(0~1023)
 *
 * 使用你的伺服馬達規格：
 *   - 0°   = 450 µs
 *   - 180° = 2450 µs
 *   - PWM Period = 20000 µs (50Hz)
 */
void Servo_WriteAngle(uint16_t angle, unsigned char prescaler)
{
    if (angle > 180)  angle = 180;
    if (angle < 0) angle = 0;

    // ===== 1. 角度 → 脈寬(µs) =====
    // pulse = (angle + 90) * (2450 - 450) / 180 + 450
    uint32_t pulse_us =
        ((uint32_t)angle * (2450 - 450)) / 180 + 450;

    // ===== 2. 脈寬 → CCP duty (10-bit) =====
    //
    // duty = pulse_us / (Tosc * prescale)
    //
    float Tosc = (1.0f / (float)_XTAL_FREQ) * 1000000.0f; // µs
    float rawCCPR = pulse_us / (Tosc * (float)prescaler); // 16 = TMR2 prescale

    uint16_t duty = (uint16_t)(rawCCPR);  // 轉成整數

    if (duty > 1023) duty = 1023;

    CCP1_SetDuty(duty);
}



// void CCP1_Initialize() {
//     TRISCbits.TRISC2=0;	// RC2 pin is output.
//     CCP1CON=9;		// Compare mode, initialize CCP1 pin high, clear output on compare match
//     PIR1bits.CCP1IF=0;
//     IPR1bits.CCP1IP = 1;
// }
