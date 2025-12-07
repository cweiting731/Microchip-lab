#ifndef _CCP1_H
#define	_CCP1_H

#include <xc.h>
#include <stdint.h>

/**
 * @brief 初始化 CCP1 模組為 PWM 模式
 *
 * @param frequency PWM 頻率 (Hz)
 *        公式：PWM_freq = Fosc / (4 * prescaler * (PR2 + 1))
 *
 * @param prescale TMR2 預分頻 (可選 1, 4, 16)
 */
void CCP1_PWM_Initialize(uint32_t frequency, uint8_t prescale);

/**
 * @brief 設定 PWM Duty (0~1023)
 *
 * @param duty 10-bit duty cycle
 */
void CCP1_SetDuty(uint16_t duty);

void Servo_WriteAngle(uint16_t angle, unsigned char prescaler);

void Servo_WritePulseUS(uint16_t pulse_us, uint8_t prescale);

// void CCP1_Initialize();
#endif	

