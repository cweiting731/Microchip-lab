#include "setting.h"
#include "config.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"
#include <stdbool.h>
#include "timers.h"
#include <stdint.h>

bool isRunning = false;
// [1428, 1535]
// < 1428 順時針
// > 1535 逆時針
const uint16_t stopMin = 1428;
const uint16_t stopMax = 1535;

// [0, 1, 2, 3, 4, 5, 6]
// [0 - 2] : 順時針
// 3 : 停止
// [4 - 6] : 逆時針
unsigned char state = 3;
unsigned char displayState = 7; // uninitialized

char str[20];
unsigned char currentMode = 0; // 0 for none, 1 for mode1, 2 for mode2

void updateLATDdigit(unsigned char digit) {
    LATD = (LATD & 0x0F) | ((digit & 0x0F) << 4);
}

// update LATD3 digit to show isRunning status
void updateIsRunningLED(bool activated) {
    if (activated) {
        LATDbits.LATD3 = 1; // Set LATD3 high
    } else {
        LATDbits.LATD3 = 0; // Set LATD3 low
    }
}

// update LATD4 to show clockwise/counter-clockwise
void updateDirectionLED(bool clockwise) {
    if (clockwise) {
        LATDbits.LATD4 = 1; // Set LATD4 high for clockwise
    } else {
        LATDbits.LATD4 = 0; // Set LATD4 low for counter-clockwise
    }
}

// update LATD5 ~ 7 digit to show current state
void updateStateLED(unsigned char state) {
    switch (state) {
        case 0:
        case 6:
            LATDbits.LATD5 = 0;
            LATDbits.LATD6 = 0;
            LATDbits.LATD7 = 1;
            break;
        case 1:
        case 5:
            LATDbits.LATD5 = 0;
            LATDbits.LATD6 = 1;
            LATDbits.LATD7 = 0;
            break;
        case 2:
        case 4:
            LATDbits.LATD5 = 1;
            LATDbits.LATD6 = 0;
            LATDbits.LATD7 = 0;
            break;
        case 3:
            LATDbits.LATD5 = 0;
            LATDbits.LATD6 = 0;
            LATDbits.LATD7 = 0;
            break;
        default:
            break;
    }

    switch (state) {
        case 0:
        case 1:
        case 2:
            updateDirectionLED(true); // Clockwise
            break;
        case 4:
        case 5:
        case 6:
            updateDirectionLED(false); // Counter-clockwise
            break;
        case 3:
            // Stop
            updateDirectionLED(false);
            break;
        default:
            break;
    }
}


void initAll() {
    
}

void commandHandler() {
    strcpy(str, GetString());
    // 檢查str長度 // 結束信號
    if (strlen(str) == 0) {
        if (currentMode == 0) { // 處理沒有mode還按enter的情況
            updateLATDdigit(0);
            ClearBuffer();
            return;
        }
        else if (currentMode == 1) {
            updateLATDdigit(0);
            UART_Write_Text("\r\nBasic Mode Exit\r\n");
            UART_Write_Text("===============\r\n");
        }
        
        currentMode = 0;
        return;
    }

    if(strncmp(str, "basic", 5) == 0){
        UART_Write_Text("===============\r\n");
        UART_Write_Text("Basic Mode Enter\r\n\r\n");
    }
    
    ClearBuffer();
}

unsigned char ADCmap() {
    uint16_t adcValue = ADC_Read(0); // Read AN0
    // Map ADC value to state

    // 0 - 1023 to 0 - 6
    unsigned char mappedState = (unsigned char)((adcValue * 7) / 1024);

    // UART_Write_Text("\b\b\b\b");
    // char buffer[10];
    // sprintf(buffer, "%4d", mappedState);
    // UART_Write_Text(buffer);

    return mappedState;
}

void main(void) 
{
    SYSTEM_Initialize() ;

    TRISBbits.TRISB0 = 1; // RB0 input
    TRISAbits.TRISA0 = 1; // AN0 input

    TRISD = 0; // RD all output
    LATD = 0; // clear LED
    
    CCP1_PWM_Initialize(50, 16); // 50Hz for servo
    ADC_Initialize(0b1110, 0, 0, 0b010, 0b100, 1); // AN0 analog, Vref=Vdd/Vss, 4Tad, Fosc/4, right justified
    // ADC_enableInterrupt();
    // ADC_startInterruptRead(0); // Start reading AN0

    // ADCON0bits.GO = 1; // Start ADC conversion
    Servo_WritePulseUS(1500, 16); // 1500us pulse to center servo

    while(1) {
        unsigned char newState = ADCmap();

        if (newState != displayState) {
            displayState = newState;
            char buffer[10];
            sprintf(buffer, "%4d\r\n", newState);
            UART_Write_Text(buffer);
            // UART_Write_Text("\b\b\b\b");
            updateStateLED(newState);
        }

        if (PORTBbits.RB0 == 0) { // Button pressed
            __delay_ms(20); // Debounce
            while (PORTBbits.RB0 == 0); // Wait for button release

            isRunning = !isRunning;
            if (isRunning) {
                updateIsRunningLED(true);
                UART_Write_Text("\r\nAuto Mode Start\r\n");
            } else {
                state = 3;
                updateIsRunningLED(false);
                updateStateLED(state);
                Servo_WritePulseUS(1500, 16); // Stop servo
                UART_Write_Text("\r\nAuto Mode Stop\r\n");
            }
            __delay_ms(20); // Debounce
        }

        if (!isRunning) {
            continue;
        }
        
        if (newState != state) {
            state = newState;
            updateStateLED(state);
            if (state < 3) {
                // 順時針
                uint16_t pulse = stopMin - (3 - state) * 100;
                Servo_WritePulseUS(pulse, 16);

                UART_Write_Text("Clockwise Pulse: ");
                char buffer[10];
                sprintf(buffer, "%4d\r\n", pulse);
                UART_Write_Text(buffer);
            } else if (state == 3) {
                // 停止
                Servo_WritePulseUS(1500, 16);
                UART_Write_Text("Stop Pulse: 1500\r\n");
            } else {
                // 逆時針
                uint16_t pulse = stopMax + (state - 3) * 100;
                Servo_WritePulseUS(pulse, 16);

                UART_Write_Text("Counter-Clockwise Pulse: ");
                char buffer[10];
                sprintf(buffer, "%4d\r\n", pulse);
                UART_Write_Text(buffer);
            }
        }
    }

    return;
}

// void interrupt low_priority Lo_ISR(void)
void __interrupt(low_priority)  Lo_ISR(void)
{   
    

    return;
}

uint16_t preADCvalue = 0;
void __interrupt(high_priority) Hi_ISR(void)
{
    if(RCIF)
    {
        if(RCSTAbits.OERR)
        {
            CREN = 0;
            Nop();
            CREN = 1;
        }
        
        MyusartRead();
    }
}