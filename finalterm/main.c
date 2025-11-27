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
unsigned char advanceTargetCount = 100;
unsigned char advanceDigitCount = 0; // 0~15

bool hardInit = 0;
unsigned char preValue = 0;
unsigned int preADCvalue = 0;

void Timer2_Initialize();
void localADC_Initialize();
unsigned int localADC_Read();
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
                
                currentMode = 0;
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
            }
            else if(strcmp(str, "advance") == 0){
                Timer2_Initialize(); // 初始化 Timer2

                currentMode = 2;
                UART_Write_Text("===============\r\n");
                UART_Write_Text("Advance Mode Enter\r\n");
                // 顯示現在delay時間
                char buf[30];
                sprintf(buf, "Switch Delay: %.1f\r\n", (float)advanceTargetCount * 0.01);
                UART_Write_Text(buf);
            }
            else if (strcmp(str, "hard") == 0) {
                localADC_Initialize();
                currentMode = 3;
                UART_Write_Text("===============\r\n");
                UART_Write_Text("Hard Mode Enter\r\n");
            }
            else if(strcmp(str, "init") == 0) {
                initAll();
                UART_Write_Text("\r\nValue Initialized\r\n");
                UART_Write_Text("===============\r\n");
            }
            ClearBuffer();
        }

        

        // 持續跑當前模式
        if(currentMode == 1) Mode1();
        else if(currentMode == 2) Mode2();
        else if (currentMode == 3) Mode3();
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
    // else if (currentMode == 3) {
    //     // handle ADC interrupt
    //     if (PIR1bits.ADIF) {

    //         unsigned char transferValue = ADCmap();
    //         updateLATDdigit(transferValue);
    //         if (hardInit == 0) {
    //             char buf[5];
    //             sprintf(buf, "%3d", transferValue);
    //             UART_Write_Text(buf);
    //             hardInit = 1;
    //         } else {
    //             UART_Write('\b');
    //             UART_Write('\b');
    //             UART_Write('\b');
    //             char buf[5];
    //             sprintf(buf, "%3d", transferValue);
    //             UART_Write_Text(buf);
    //         }

    //         PIR1bits.ADIF = 0; // Clear ADC interrupt flag
    //         ADCON0bits.GO = 1;      // start next conversion
    //     }
    // }
    
   // process other interrupt sources here, if required
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

void localADC_Initialize() {
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    TRISAbits.RA0 = 1;       // AN0 input
    ADCON1bits.PCFG = 0b1110; // only AN0 analog
    ADCON0bits.CHS  = 0b0000; // channel 0 = AN0

    ADCON2bits.ADCS = 0b100; // Tosc * 4 = 0.25 µs * 4 = 1 µs = Tad (> 0.7 µs)
    ADCON2bits.ACQT = 0b010; // 4 Tad, Tacq = 4 µs (> 2.4 µs)
    ADCON2bits.ADFM = 1;      // right justified (建議用右對齊)
    ADCON0bits.ADON = 1;      // turn on ADC

    // PIE1bits.ADIE = 1; // Enable ADC interrupt
    // PIR1bits.ADIF = 0; // Clear ADC interrupt flag

    // INTCONbits.PEIE = 1; // Enable peripheral interrupt
    // INTCONbits.GIE = 1; // Enable global interrupt

    // ADCON0bits.GO = 1;      // start ADC conversion
}

unsigned int localADC_Read() {
    __delay_us(5);             // acquisition time
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO);      // wait completion
    return ((ADRESH << 8) | ADRESL);
}

unsigned char ADCmap() {
    unsigned int adcValue = localADC_Read();  // 0~1023
    if (hardInit == 0) {
        preADCvalue = adcValue;
    }
    else {
        if (abs((int)adcValue - (int)preADCvalue) < 4) {
            return 0;
        } else {
            preADCvalue = adcValue;
        }
    }
    
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