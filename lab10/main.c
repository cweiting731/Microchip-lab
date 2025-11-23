#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"
#include <stdbool.h>
// using namespace std;

#define _XTAL_FREQ 4000000UL

char str[20];
unsigned char currentMode = 0; // 0 for none, 1 for mode1, 2 for mode2

unsigned char basicCounter = 0;
bool basicInit = 0;

unsigned char advanceTimer2Count = 0; // 10 = 0.1s
unsigned char advanceTargetCount = 200;
unsigned char advanceDigitCount = 0; // 0~15

void Timer2_Initialize();

void updateLATDdigit(unsigned char digit) {
    LATD = (LATD & 0x0F) | ((digit & 0x0F) << 4);
}

void initAll() {
    basicCounter = 0;
    basicInit = 0;

    advanceDigitCount = 0;
    advanceTargetCount = 100; // 1s
}

void Mode1()  // Basic mode
{

    // ==== 1. 按鈕去彈跳 ====
    if (PORTBbits.RB0 == 0)   // 按下（假設拉高，按下=0）
    {
        __delay_ms(20);       // debounce
        // 等待按鍵放開，避免一次按多次
        while (PORTBbits.RB0 == 0);

        basicCounter++;        // 計數器 +1
        
        UART_Write('\b');
        UART_Write('\b');
        UART_Write('\b');
        
        char buf[5];
        sprintf(buf, "%3d", basicCounter);
        UART_Write_Text(buf);

        updateLATDdigit(basicCounter);

        __delay_ms(20);
    }
    
    // init
    if (!basicInit) {
        char buf[5];
        sprintf(buf, "%3d", basicCounter);
        UART_Write_Text(buf);

        LATD = (LATD & 0x0F) | (basicCounter & 0x0F) << 4;
        basicInit = 1; // done init
    }
    
    return;
}

void Mode2() {   // Todo : Mode2 
    return ;
}
void main(void) 
{
    
    SYSTEM_Initialize() ;

    TRISBbits.TRISB0 = 1; // RB0 input

    TRISD = 0; // RD all output
    LATD = 0; // clear LED
    
    while(1) {
        if (IsCommandReady()) {
            ClearCommandReady();

            strcpy(str, GetString());

            // 檢查str長度 // 結束信號
            if (strlen(str) == 0) {
                if (currentMode == 0) { // 處理沒有mode還按enter的情況
                    updateLATDdigit(0);
                    ClearBuffer();
                    continue;
                }
                else if (currentMode == 1) {
                    updateLATDdigit(0);
                    UART_Write_Text("\r\nBasic Mode Exit\r\n");
                    UART_Write_Text("===============\r\n");
                }
                else if (currentMode == 2) {
                    PIE1bits.TMR2IE = 0; // Disable Timer2 interrupt
                    IPR1bits.RCIP = 0; // UART interrupt back to low priority
                    updateLATDdigit(0);

                    UART_Write_Text("\r\nAdvance Mode Exit\r\n");
                    UART_Write_Text("===============\r\n");
                }
                
                currentMode = 0;
                ClearBuffer();
                continue;
            }

            // 攔截advance的浮點數輸入
            // 檢查當前mode是2 且 輸入是浮點數
            if (currentMode == 2) {
                char *endptr;
                float val = strtof(str, &endptr);
                if (endptr != str && *endptr == '\0') {
                    // 成功轉換為浮點數
                    char buf[30];
                    advanceTargetCount = (unsigned char)(val * 100); // 0.1s為單位
                    sprintf(buf, "Switch Delay: %.1f\r\n", val);
                    UART_Write_Text(buf);
                    ClearBuffer();
                    continue;
                }
            }

            if(strcmp(str, "basic") == 0){
                currentMode = 1;
                basicInit = 0;      // reset Mode1 display
                UART_Write_Text("===============\r\n");
                UART_Write_Text("Basic Mode Enter\r\n\r\n");
                ClearBuffer();
            }
            else if(strcmp(str, "advance") == 0){
                IPR1bits.RCIP = 1; // UART interrupt high priority
                Timer2_Initialize(); // 初始化 Timer2

                currentMode = 2;
                UART_Write_Text("===============\r\n");
                UART_Write_Text("Advance Mode Enter\r\n");
                // 顯示現在delay時間
                char buf[30];
                sprintf(buf, "Switch Delay: %.1f\r\n", (float)advanceTargetCount * 0.01);
                UART_Write_Text(buf);
                ClearBuffer();
            }
            else if(strcmp(str, "init" == 0)) {
                initAll();
                UART_Write_Text("\r\nValue Initialized\r\n");
                UART_Write_Text("===============\r\n");
                ClearBuffer();
            }
        }

        

        // 持續跑當前模式
        if(currentMode == 1) Mode1();
        else if(currentMode == 2) Mode2();
    }

    return;
}

// void interrupt low_priority Lo_ISR(void)
void __interrupt(low_priority)  Lo_ISR(void)
{
    if (currentMode == 2) {
        if (PIR1bits.TMR2IF) {
            PIR1bits.TMR2IF = 0; // Clear Timer2 interrupt flag

            advanceTimer2Count++;
            if (advanceTimer2Count >= advanceTargetCount) {
                advanceTimer2Count = 0;

                advanceDigitCount = (advanceDigitCount + 1) % 16;
                updateLATDdigit(advanceDigitCount);

                // 輸出advanceDigitCount到UART
                // char buf[40];
                // sprintf(buf, "Advance Digit Count: %d | %.1fs\r\n", advanceDigitCount, (float)advanceTargetCount * 0.01);
                // UART_Write_Text(buf);
                // UART_Write_Text("Advance Mode: 1 second passed\r\n");
            }
        }
    }
    else {
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
    
   // process other interrupt sources here, if required
    return;
}

void __interrupt(high_priority) Hi_ISR(void)
{
    if (currentMode == 2) {
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
}

void Timer2_Initialize()
{
    // Timer2 off first
    T2CONbits.TMR2ON = 0;

    // Prescaler = 1:16
    T2CONbits.T2CKPS = 0b11;

    // Postscaler = 1:16
    T2CONbits.T2OUTPS = 0b1111;

    // 10ms period (0.01s)
    PR2 = 38;

    // Clear counter
    TMR2 = 0;

    // Clear interrupt flag
    PIR1bits.TMR2IF = 0;

    // Enable Timer2 interrupt
    PIE1bits.TMR2IE = 1;

    // Timer2 interrupt = LOW PRIORITY (題目要求 UART > Timer2)
    IPR1bits.TMR2IP = 0;

    // Turn on Timer2
    T2CONbits.TMR2ON = 1;
}
