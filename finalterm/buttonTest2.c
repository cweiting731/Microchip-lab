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

void setupButtonInterrupt()
{
    // ===== 啟用中斷優先權 =====
    RCONbits.IPEN = 1;          // 等同 assembly: BSF RCON, IPEN

    // ===== RB0 設成輸入 =====
    TRISBbits.TRISB0 = 1;       // 等同 assembly: BSF TRISB,0
    INTCON2bits.RBPU = 0;       // 開啟 PORTB pull-up（你的 ASM 也有）
    ADCON1bits.PCFG = 0b1110; // AN0 analog, AN1~AN12 digital

    // ===== INT0 邊緣觸發 =====
    INTCON2bits.INTEDG0 = 0;    // 下降沿觸發（按下=0）等同 ASM: BCF INTCON2, INTEDG0

    // ===== 清除中斷旗標 =====
    INTCONbits.INT0IF = 0;      // 等同 ASM: BCF INTCON, INT0IF

    // ===== 啟用 INT0 Interrupt =====
    INTCONbits.INT0IE = 1;      // 等同 ASM: BSF INTCON, INT0IE

    // ===== 啟用 High Priority Interrupt =====
    INTCONbits.GIEH = 1;        // 等同 ASM: BSF INTCON, GIEH

    // ===== 啟用 Low Priority Interrupt（若需要 Timer0/ADC）=====
    INTCONbits.GIEL = 1;        // 等同 ASM: BSF INTCON, GIEL
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

unsigned int angle = 180;
bool isRunning = false;
void main(void) 
{
    SYSTEM_Initialize() ;

    setupButtonInterrupt();
    // TRISBbits.TRISB0 = 1; // RB0 input
    // ADCON1bits.PCFG = 0b1110; // AN0 analog, AN1~AN12 digital

    // TRISAbits.TRISA0 = 1; // AN0 input

    TRISD = 0; // RD all output
    LATD = 0; // clear LED
    
    CCP1_PWM_Initialize(50, 16); // 50Hz for servo
    Servo_WriteAngle(angle, 16); // initial position
    // Timer0_Initialize(53035, 8, 0);
    // ADC_Initialize(0b1110, 0, 0, 0b010, 0b100, 1); // AN0 analog, Vref=Vdd/Vss, 4Tad, Fosc/4, right justified
    // ADC_enableInterrupt();
    // ADC_startInterruptRead(0); // Start reading AN0


    // ADCON0bits.GO = 1; // Start ADC conversion
    UART_Write_Text("initialized\r\n");
    while(1) {
        // if (IsCommandReady()) {
        //     ClearCommandReady();
        //     commandHandler();
        // }
        // if (PORTBbits.RB0 == 0) {
        //     __delay_ms(50); // simple debounce
        //     while (PORTBbits.RB0 == 0); // wait for button release
        //     angle += 90;
        //     if (angle > 180) angle = 0;
        //     Servo_WriteAngle(angle, 16);

        //     __delay_ms(50); // simple debounce
        // }


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
    // if (Timer0_isInterruptFlagSet()) {
    //     Timer0_clearInterruptFlag();
    //     Timer0_reload(53035); // Reload Timer0 preload value
        
    //     if (isRunning) {
    //         angle += 5;
    //         if (angle > 180) angle = 0;
    //         Servo_WriteAngle(angle, 16); // Update servo angle
    //     }
    // }
    
    return;
}
// uint16_t preADCvalue = 0;
void __interrupt(high_priority) Hi_ISR(void)
{
    UART_Write_Text("interrupt\r\n");
    if (INTCONbits.INT0IF) {
        __delay_ms(50); // simple debounce
        while (PORTBbits.RB0 == 0); // wait for button release
        // isRunning = !isRunning; // toggle isRunning state   
        // if (isRunning) {
        //     updateLATDdigit(15);
        // } else {
        //     updateLATDdigit(0);
        // }
        angle += 90;
        if (angle > 180) angle = 0;
        Servo_WriteAngle(angle, 16);
        char buf[5];
        sprintf(buf, "%3d", angle);
        UART_Write_Text(buf);
        UART_Write_Text("\r\n");

        __delay_ms(50); // simple debounce
        INTCONbits.INT0IF = 0; // clear INT0 flag
    }

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