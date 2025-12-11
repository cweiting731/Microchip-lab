#include "setting.h"
#include "config.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"
#include <stdbool.h>
#include "timers.h"
#include <stdint.h>
// using namespace std;

char str[20];
unsigned char currentMode = 0; // 0 for none, 1 for mode1, 2 for mode2

void updateLATDdigit(unsigned char digit) {
    LATD = (LATD & 0x0F) | ((digit & 0x0F) << 4);
}

bool isRunning = false;


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

uint16_t pulse = 1425;

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
    Servo_WritePulseUS(pulse, 16); // 1500us pulse to center servo

    while(1) {
        if (PORTBbits.RB0 == 0) { // Button pressed
            __delay_ms(20); // Debounce
            while (PORTBbits.RB0 == 0); // Wait for button release

            pulse++;
            Servo_WritePulseUS(pulse, 16);
            UART_Write_Text("Pulse: ");
            char buffer[10];
            sprintf(buffer, "%4d\r\n", pulse);
            UART_Write_Text(buffer);

            // isRunning = !isRunning;
            // if (isRunning) {
            //     UART_Write_Text("\r\nAuto Mode Start\r\n");
            // } else {
            //     UART_Write_Text("\r\nAuto Mode Stop\r\n");
            // }
            __delay_ms(20); // Debounce
        }

        // if (!isRunning) {
        //     continue;
        // }

        // uint16_t adcValue = ADC_Read(0); // Read AN0
        // // ====== ADC → PWM (1000~2000us) ======
        // uint16_t pulse = 1000 + ((uint32_t)adcValue * 1000 / 1023);
        // Servo_WritePulseUS(pulse, 16);
        // UART_Write_Text("\b\b\b\b");
        // char buffer[10];
        // sprintf(buffer, "%4d", pulse);
        // UART_Write_Text(buffer);
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