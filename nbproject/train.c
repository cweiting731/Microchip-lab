// === Configuration Bits ===
#pragma config OSC = INTIO67
#pragma config PWRT = OFF
#pragma config BOREN = ON
#pragma config WDT = OFF
#pragma config PBADEN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF

#include <stdio.h>
#include <string.h>
#include <builtins.h>

#define _XTAL_FREQ 4000000  // Internal Clock speed
#include "lib.h"

bool isRunning = false;
int step = 1; // 1: clockwise, -1: counter-clockwise
unsigned int servoAngle = 0; // 0 ~ 180
unsigned int stepDelay = 20; // ms

void delay_ms_var(unsigned int ms) {
    while (ms--) {
        __delay_ms(1);
    }
}

unsigned int delayArr[] = {1, 10, 20, 30, 40, 50, 60, 70};

void __interrupt(low_priority) L_ISR(void) {
    if (interruptByADConverter()) {

        unsigned int adcVal = getADConverter();   // 0 ~ 1023
        clearInterrupt_ADConverter();

        // === 把 ADC 轉換成速度 ===
        // 速度 delay 範圍：10ms ~ 80ms（你可自行調整）
        stepDelay = delayArr[adcVal / 128];   // 0→10ms, 1023→~73ms

        startADConverter();    // 再開始下一次轉換
    }
}


void __interrupt(high_priority) H_ISR() {
    if (interruptByRB0External()) {
        isRunning = !isRunning;
        digitalWrite(PIN_RC0, isRunning ? 1 : 0);
        __delay_ms(20); // Debounce delay
        clearInterrupt_RB0External();
    }
}

void main(void) {
    setIntrnalClock();
    setPortBPullup(PORTB_PULLUP_ENABLE);

    enableInterruptPriorityMode(1);  // enable the priority interrupt
    enableGlobalInterrupt(1);        // enable the global interrupt
    enablePeripheralInterrupt(1);    // enable peripheral interrupt

    // Button
    pinMode(PIN_RB0, PIN_INPUT);
    enableInterrupt_RB0External();  // enable RB0 interrupt

    // LED
    pinMode(PIN_RC0, PIN_OUTPUT);
    digitalWrite(PIN_RC0, 0); // Turn off LED initially

    // Servo
    pinMode(PIN_RC2, PIN_OUTPUT);              // Set servo pin CCP1 output
    digitalWrite(PIN_RC2, 0);                  // Set servo pin CCP1 to low
    enableTimer2(TIMER2_PRESCALE_16, 0b0000);  // Enable Timer2 for PWM control
    setTimer2InterruptPeriod(4100, 16, 1);     // Setup servo report rate, 4.1ms
    setCCP1Mode(ECCP_MODE_PWM_HH);             // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    setCCP1ServoAngle(servoAngle, 16);              // Reset angle to 0

    // Potentiometer
    pinMode(PIN_RA0, PIN_INPUT);
    setANPinVoltageReferenceConfig(0, 0);
    setANPinADConfig(0b1110);        // AN0 analog
    setANPinAnalogChannelSelect(0);  // AN0 channel select
    enableADConverter();             // enable A/D Converter module
    enableInterrupt_ADConverter(1);  // enable ADC interrupt

    setCCP1ServoAngle(servoAngle, 16); // Initialize servo position
    startADConverter();               // Start first A/D conversion

    while(1) {
        if (isRunning) {
            servoAngle += step;
            if (servoAngle >= 180 || servoAngle <= 0) {
                step = -step; // Change direction
            }
            setCCP1ServoAngle(servoAngle, 16); // Update servo angle
            delay_ms_var(stepDelay); // Delay based on ADC value
        }
    }
}