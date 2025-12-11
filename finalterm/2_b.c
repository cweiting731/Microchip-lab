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
unsigned char state = 0;
bool isAdder = false;
long long n1 = 0;
long long n2 = 0;
bool needToInit = false;
void commandHandler() {
    strcpy(str, GetString());
    // 檢查str長度 // 結束信號
    if (strlen(str) == 0) {
        if (needToInit) {
            for (int i=0; i<100; i++) {
                UART_Write('\b');
            }
            for (int i=0; i<100; i++) {
                UART_Write(' ');
            }
            for (int i=0; i<100; i++) {
                UART_Write('\b');
            }
            state = 0;
            n1 = 0;
            n2 = 0;
            isAdder = false;
            needToInit = false;
            updateLATDdigit(0);
        }
        ClearBuffer();
        return;
    }

    if (isAdder) {
        long long result = n1 + n2;
        char buf[30];
        sprintf(buf, "%10lld", result);
        UART_Write_Text(buf);
        updateLATDdigit(result % 16);
        needToInit = true;
    } else {
        long long result = n1 * n2;
        char buf[30];
        sprintf(buf, "%10lld", result);
        UART_Write_Text(buf);
        updateLATDdigit(result % 16);
        needToInit = true;
    }
    
    ClearBuffer();
}


void main(void) 
{
    SYSTEM_Initialize() ;

    TRISBbits.TRISB0 = 1; // RB0 input
    // TRISAbits.TRISA0 = 1; // AN0 input

    TRISD = 0; // RD all output
    LATD = 0; // clear LED
    

    while(1) {
        if (IsCommandReady()) {
            ClearCommandReady();
            commandHandler();
        }
    }

    return;
}

// void interrupt low_priority Lo_ISR(void)
void __interrupt(low_priority)  Lo_ISR(void)
{   
    return;
}

// uint16_t preADCvalue = 0;
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
        
        char c = MyusartRead();
        // UART_Write_Text("\r\nISR received: ");
        // UART_Write(c);
        // UART_Write_Text("\r\n");

        if (c == '+') {
            isAdder = true;
            state++;
            updateLATDdigit(0);
            ClearBuffer();
            return;
        }
        if (c == '*') {
            isAdder = false;
            state++;
            updateLATDdigit(0);
            ClearBuffer();
            return;
        }

        if (state == 0) {
            strcpy(str, GetString());
            char *endptr;
            float val = strtof(str, &endptr);
            if (endptr != str && *endptr == '\0') {
                // 成功轉換為浮點數
                char buf[30];
                n1 = (unsigned int)val;
                updateLATDdigit(n1 % 16);
                return;
            }
        } else if (state == 1) {
            strcpy(str, GetString());
            char *endptr;
            float val = strtof(str, &endptr);
            if (endptr != str && *endptr == '\0') {
                // 成功轉換為浮點數
                char buf[30];
                n2 = (unsigned int)val;
                updateLATDdigit(n2 % 16);
                return;
            }
        }
    }
}