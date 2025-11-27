#include "setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"
#include <stdbool.h>
#include "timers.h"

#define _XTAL_FREQ 4000000UL

unsigned int advanceTimer2Count = 0; // 10 = 0.1s
unsigned int advanceTargetCount = 100;
unsigned int advanceDigitCount = 0;

void updateLATDdigit(unsigned char digit) {
    LATD = (LATD & 0x0F) | ((digit & 0x0F) << 4);
}

void main() {
    SYSTEM_Initialize() ;

    TRISBbits.TRISB0 = 1; // RB0 input

    TRISD = 0; // RD all output
    LATD = 0; // clear LED
    Timer1_Initialize(64285, 8, 0); // 初始化 Timer1
    char buf[40];
    sprintf(buf, "Timer1 Preload: %u %02X %02X\r\n", 64285, TMR1H, TMR1L);
    UART_Write_Text(buf);
    while (1) {

    }
}

void __interrupt(low_priority)  Lo_ISR(void)
{
    if (PIR1bits.TMR1IF) {
        PIR1bits.TMR1IF = 0; // Clear Timer1 interrupt flag
        // 印出advanceTimer2Count
        // char buf[20];
        // sprintf(buf, "%3d %d %d\r\n", advanceTimer2Count, TMR1H, TMR1L);
        // UART_Write_Text(buf);

        advanceTimer2Count++;
        if (advanceTimer2Count >= advanceTargetCount) {
            advanceTimer2Count = 0;

            advanceDigitCount = (advanceDigitCount + 1) % 16;
            updateLATDdigit(advanceDigitCount);

            // 輸出advanceDigitCount到UART
            char buf[40];
            sprintf(buf, "Advance Digit Count: %d | %.1fs\r\n", advanceDigitCount, (float)advanceTargetCount * 0.01);
            UART_Write_Text(buf);
            UART_Write_Text("Advance Mode: 1 second passed\r\n");
        }
    }

    return;
}