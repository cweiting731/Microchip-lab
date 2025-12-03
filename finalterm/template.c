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

void initAll() {
    
}

void Mode1()  // Basic mode
{
    return ;
}

void Mode2() {   // Todo : Mode2 
    return ;
}

void Mode3() {   // Todo : Mode3 
    return ;
}

void Mode4() {
    return ;
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

void main(void) 
{
    SYSTEM_Initialize() ;

    TRISBbits.TRISB0 = 1; // RB0 input
    TRISAbits.TRISA0 = 1; // AN0 input

    TRISD = 0; // RD all output
    LATD = 0; // clear LED
    
    CCP1_PWM_Initialize(50, 16); // 50Hz for servo
    ADC_Initialize(0b1110, 0, 0, 0b010, 0b100, 1); // AN0 analog, Vref=Vdd/Vss, 4Tad, Fosc/4, right justified
    ADC_enableInterrupt();
    ADC_startInterruptRead(0); // Start reading AN0


    ADCON0bits.GO = 1; // Start ADC conversion

    while(1) {
        if (IsCommandReady()) {
            ClearCommandReady();
            commandHandler();
        }

        // 持續跑當前模式
        if(currentMode == 1) Mode1();
        else if(currentMode == 2) Mode2();
        else if (currentMode == 3) Mode3();
        else if (currentMode == 4) Mode4();
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

    if (PIR1bits.ADIF) {
        uint16_t adcValue = ((uint16_t)ADRESH << 8) | ADRESL;
        if (adcValue > preADCvalue + 15 || adcValue < preADCvalue - 15) {
            preADCvalue = adcValue;
        } else {
            ADC_clearInterruptFlag();
            // Start next ADC conversion
            ADC_startInterruptRead(0);
            return;
        }

        // Convert ADC value (0-1023) to servo angle (0-180)
        uint16_t angle = (uint32_t)adcValue * 180 / 1023;

        // Write angle to servo
        Servo_WriteAngle(angle, 16); // prescaler = 16

        ADC_clearInterruptFlag();
        // Start next ADC conversion
        ADC_startInterruptRead(0);
    }
}