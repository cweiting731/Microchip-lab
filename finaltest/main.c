#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <builtins.h>

#define _XTAL_FREQ 4000000  // Internal Clock speed
#include "lib.h"
#include "uart.h"

#pragma config OSC = INTIO67  // Oscillator Selection bits
#pragma config WDT = OFF      // Watchdog Timer Enable bit
#pragma config PWRT = OFF     // Power-up Enable bit
#pragma config BOREN = ON     // Brown-out Reset Enable bit
#pragma config PBADEN = OFF   // Watchdog Timer Enable bit
#pragma config LVP = OFF      // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF      // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)

void UART_Initialize();

void onReadLine(char *line, byte len) {
    if (strncmp("mode0", line, len)) {
        // do stuff...
    }
    serialPrint("Received line: ");
    serialPrint(line);
    serialPrint("\r\n");
}
void onReadChar(char c) {
    byte num = c - '0';
}
void __interrupt(low_priority) Lo_ISR(void) {
    if (processSerialReceive()) // Essential for serial read listener
        return;
}
void main(void) {
    // System init
    setIntrnalClock();
    setPortBPullup(PORTB_PULLUP_ENABLE);
    enableInterruptPriorityMode(1);  // enable the priority interrupt
    enableGlobalInterrupt(1);        // enable the global interrupt
    enablePeripheralInterrupt(1);    // enable peripheral interrupt

    
}