#include <xc.h>
#include <stdbool.h>
    //setting TX/RX

char mystring[20];
int lenStr = 0;
bool uartCommandReady = false;

void UART_Initialize() {
           
    /* TODObasic   
        Serial Setting      
        1.   Setting Baud rate
        2.   choose sync/async mode 
        3.   enable Serial port (configures RX/DT and TX/CK pins as serial port pins)
        3.5  enable Tx, Rx Interrupt(optional)
        4.   Enable Tx & RX
    */   
    TRISCbits.TRISC6 = 1;            
    TRISCbits.TRISC7 = 1;            
    
    //  Setting baud rate
    TXSTAbits.SYNC = 0;           
    BAUDCONbits.BRG16 = 0;          
    TXSTAbits.BRGH = 0;
    SPBRG = 51;      
    
   //   Serial enable
    RCSTAbits.SPEN = 1;

    PIR1bits.TXIF = 0;
    PIR1bits.RCIF = 0;

    TXSTAbits.TXEN = 1;           
    RCSTAbits.CREN = 1;

    PIE1bits.TXIE = 0;       
    IPR1bits.TXIP = 0;

    PIE1bits.RCIE = 1;              
    IPR1bits.RCIP = 0; // High priority             
}

void UART_Write(unsigned char data)  // Output on Terminal
{
    while(!TXSTAbits.TRMT);
    TXREG = data;              //write to TXREG will send data 
}


void UART_Write_Text(char* text) { // Output on Terminal, limit:10 chars
    for(int i=0;text[i]!='\0';i++)
        UART_Write(text[i]);
}

void ClearBuffer(){
    for(int i = 0; i < 10 ; i++)
        mystring[i] = '\0';
    lenStr = 0;
}

void MyusartRead()
{
    char c = RCREG;  // read received byte

    if (c == '\r') {   // Enter (carriage return)
        mystring[lenStr] = '\0';
        uartCommandReady = true;   // command is ready!
        UART_Write('\r');
        UART_Write('\n');
        return;
    }

    else if (c == '\b') {  // Backspace
        if (lenStr > 0) {
            lenStr--;
            mystring[lenStr] = '\0';

            // 視覺回饋：在 terminal 上刪除字元
            UART_Write('\b');
            UART_Write(' ');
            UART_Write('\b');
        }
        return;
    }

    else {
        if (lenStr < 19) { // prevent overflow
            mystring[lenStr++] = c;
            mystring[lenStr] = '\0';
            UART_Write(c); // echo 回 terminal
        }
    }

    return ;
}

char *GetString(){
    return mystring;
}

bool IsCommandReady(){
    return uartCommandReady;
}

void ClearCommandReady(){
    uartCommandReady = false;
}