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

unsigned int angle = 90;
bool isIncreasing = true;
unsigned int digit = 0;

void main(void) 
{
    SYSTEM_Initialize() ;

    TRISBbits.TRISB0 = 1; // RB0 input
    ADCON1bits.PCFG = 0b1110; // AN0 analog, AN1~AN12 digital

    TRISD = 0; // RD all output
    LATD = 0; // clear LED
    
    CCP1_PWM_Initialize(50, 16); // 50Hz for servo

    Servo_WriteAngle(angle, 16); // 初始角度90度
    updateLATDdigit(digit);

    while(1) {
        // ==== 1. 按鈕去彈跳 ====
        if (PORTBbits.RB0 == 0)   // 按下（假設拉高，按下=0）
        {
            __delay_ms(20);       // debounce
            // 等待按鍵放開，避免一次按多次
            while (PORTBbits.RB0 == 0);

            if (isIncreasing) {
                angle += 45;
                if (angle >= 180) {
                    angle = 180;
                    isIncreasing = false;
                }
            }
            else {
                angle -= 45;
                if (angle <= 0) {
                    angle = 0;
                    isIncreasing = true;
                }
            }
            Servo_WriteAngle(angle, 16);

            digit++;
            if (digit > 15) digit = 0;
            updateLATDdigit(digit);

            __delay_ms(20);
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