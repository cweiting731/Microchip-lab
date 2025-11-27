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

unsigned char basicCounter = 0;
bool basicInit = 0;

unsigned char advanceTimer2Count = 0; // 10 = 0.1s
unsigned char advanceTargetCount = 100;
unsigned char advanceDigitCount = 0; // 0~15

bool hardInit = 0;
unsigned char preValue = 0;

unsigned char ADCmap();

void updateLATDdigit(unsigned char digit) {
    LATD = (LATD & 0x0F) | ((digit & 0x0F) << 4);
}

void initAll() {
    basicCounter = 0;
    basicInit = 0;

    advanceDigitCount = 0;
    advanceTargetCount = 100; // 1s

    hardInit = 0;
    preValue = 0;
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

void Mode3() {   // Todo : Mode3 
    unsigned char transferValue = ADCmap();
    if (transferValue == 0) return ;

    if (hardInit == 0) {
        char buf[5];
        sprintf(buf, "%3d", transferValue);
        UART_Write_Text(buf);
        updateLATDdigit(transferValue);
        hardInit = 1;
    } else {
        if (transferValue == preValue) return ;
        UART_Write('\b');
        UART_Write('\b');
        UART_Write('\b');
        char buf[5];
        sprintf(buf, "%3d", transferValue);
        UART_Write_Text(buf);
        updateLATDdigit(transferValue);
    }
    preValue = transferValue;
    return ;
}

bool servoInit = 0;
uint16_t preADCvalue = 0;
unsigned char servoAngle = 0;
void Mode4() {
    uint16_t adcValue = ADC_Read(0); // 讀取 AN0 的 ADC 值
    if (servoInit == 0) {
        char buf[20];
        sprintf(buf, "%4d", adcValue);
        UART_Write_Text(buf);
        servoInit = 1;
        preADCvalue = adcValue;
        uint16_t angle = (uint32_t)adcValue * 180 / 1023;
        Servo_WriteAngle(angle);
    } else {
        if (adcValue == preADCvalue) return ;
        UART_Write('\b');
        UART_Write('\b');
        UART_Write('\b');
        UART_Write('\b');
        char buf[20];
        sprintf(buf, "%4d", adcValue);
        UART_Write_Text(buf);
        preADCvalue = adcValue;
        // // 將 ADC 值映射到 0~180 度
        uint16_t angle = (uint32_t)adcValue * 180 / 1023;
        Servo_WriteAngle(angle);
    }
    // uint16_t angle = (uint32_t)preADCvalue * 180 / 1023;
    // Servo_WriteAngle(angle);
    // __delay_ms(100);
    return ;

    // if (PORTBbits.RB0 == 0)   // 按下（假設拉高，按下=0）
    // {
    //     __delay_ms(20);       // debounce
    //     // 等待按鍵放開，避免一次按多次
    //     while (PORTBbits.RB0 == 0);
        
    //     servoAngle = (servoAngle + 15) % 180;
    //     Servo_WriteAngle(servoAngle);

    //     __delay_ms(20);
    // }
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
        else if (currentMode == 2) {
            Timer0_stop();
            updateLATDdigit(0);

            UART_Write_Text("\r\nAdvance Mode Exit\r\n");
            UART_Write_Text("===============\r\n");
        }
        else if (currentMode == 3) {
            updateLATDdigit(0);
            hardInit = 0; // reset hard mode init
            UART_Write_Text("\r\nHard Mode Exit\r\n");
            UART_Write_Text("===============\r\n");
        }
        else if (currentMode == 4) {
            updateLATDdigit(0);
            servoInit = 0; // reset servo mode init
            UART_Write_Text("\r\nServo Mode Exit\r\n");
            UART_Write_Text("===============\r\n");
        }
        
        currentMode = 0;
        return;
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
            return;
        }
    }

    if(strncmp(str, "basic", 5) == 0){
        currentMode = 1;
        basicInit = 0;      // reset Mode1 display
        UART_Write_Text("===============\r\n");
        UART_Write_Text("Basic Mode Enter\r\n\r\n");
    }
    else if(strncmp(str, "advance", 7) == 0){
        Timer0_Initialize(64285, 8, 0); // 初始化 Timer0

        currentMode = 2;
        UART_Write_Text("===============\r\n");
        UART_Write_Text("Advance Mode Enter\r\n");
        // 顯示現在delay時間
        char buf[30];
        sprintf(buf, "Switch Delay: %.1f\r\n", (float)advanceTargetCount * 0.01);
        UART_Write_Text(buf);
    }
    else if (strncmp(str, "hard", 4) == 0) {
        // ADC_Initialize(0b1110, 0, 0, 0b010, 0b100, 1); // AN0 analog, Vref=Vdd/Vss, 4Tad, Fosc/4, right justified
        currentMode = 3;
        UART_Write_Text("===============\r\n");
        UART_Write_Text("Hard Mode Enter\r\n");
    }
    else if (strncmp(str, "servo", 5) == 0) {
        // ADC_Initialize(0b1110, 0, 0, 0b010, 0b100, 1); // AN0 analog, Vref=Vdd/Vss, 4Tad, Fosc/4, right justified
        CCP1_PWM_Initialize(50, 16); // 50Hz for servo
        currentMode = 4;
        UART_Write_Text("===============\r\n");
        UART_Write_Text("Servo Mode Enter\r\n");
    }
    else if(strcmp(str, "init") == 0) {
        initAll();
        UART_Write_Text("\r\nValue Initialized\r\n");
        UART_Write_Text("===============\r\n");
    }
    ClearBuffer();
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
    if (currentMode == 2) {
        if (Timer0_isInterruptFlagSet()) {
            Timer0_clearInterruptFlag(); // Clear Timer0 interrupt flag
            Timer0_reload(64285); // Reload Timer0 preload value
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
                // char buf[40];
                // sprintf(buf, "Advance Digit Count: %d | %.1fs\r\n", advanceDigitCount, (float)advanceTargetCount * 0.01);
                // UART_Write_Text(buf);
                // UART_Write_Text("Advance Mode: 1 second passed\r\n");
            }
        }
    }

    return;
}

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

unsigned char ADCmap() {
    unsigned int adcValue = ADC_Read(0);  // 0~1023
    
    if (adcValue < 85) return 4;
    else if (adcValue < 170) return 5;
    else if (adcValue < 256) return 6;
    else if (adcValue < 341) return 7;
    else if (adcValue < 426) return 8;
    else if (adcValue < 512) return 9;
    else if (adcValue < 597) return 10;
    else if (adcValue < 682) return 11;
    else if (adcValue < 767) return 12;
    else if (adcValue < 852) return 13;
    else if (adcValue < 938) return 14;
    else return 15;
}