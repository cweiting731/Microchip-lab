#ifndef CCP2_H
#define CCP2_H

#include <stdint.h>

void CCP2_PWM_Initialize(uint32_t frequency, uint8_t prescale);
void CCP2_SetDuty(uint16_t duty);
void Servo2_WriteAngle(uint16_t angle, uint8_t prescale);
void Servo2_WritePulseUS(uint16_t pulse_us, uint8_t prescale);

#endif
