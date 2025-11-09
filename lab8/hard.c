// === Configuration Bits ===
// 以下是 PIC18F4520 的組態位元 (config bits)，燒進晶片的 fuses 設定一次就固定

#pragma config OSC = INTIO67   // 使用內部振盪器 (Internal OSC)，RA6/RA7 當一般 I/O
#pragma config PWRT = OFF      // Power-up Timer 關閉，上電後不特別延遲
#pragma config BOREN = ON      // Brown-out Reset 開啟，電壓過低時自動重置
#pragma config WDT = OFF       // Watchdog Timer 關閉（不使用硬體看門狗）
#pragma config PBADEN = OFF    // PORTB<4:0> 上電預設為數位 I/O（而不是類比）
#pragma config LVP = OFF       // Low-Voltage Programming 關閉，避免 RB5 被當成程式腳
#pragma config CPD = OFF       // Data EEPROM 不做程式保護

#include <xc.h>
#include <pic18f4520.h>

// 這個數值給 __delay_ms() / __delay_us() 用，代表 Fosc (震盪頻率)
// 你現在把 IRCF 設成 0b001 → Fosc = 125 kHz，所以要寫 125000
#define _XTAL_FREQ 125000UL   // 125 kHz internal clock

// === 全域變數 ===
// 這三個在主程式和中斷都會用，所以設成 global + volatile

volatile unsigned char ccpr = 15;   // 用來設定 PWM duty 的 10-bit 值 (簡化成 0~127 區間的低 7 bit)
                                     // 你約定：15 ~ 75 對應伺服從最左到最右 (-90° ~ +90°)

volatile unsigned char sign = 1;    // 方向旗標：1 表示往一邊走 (++ccpr)，0 表示往另一邊 (--ccpr)
volatile unsigned char running = 0; // 狀態旗標：0 = 停止，1 = 正在自動擺動

// === 函式宣告 ===
void setServoAngle(unsigned char ccpr);
void setupPWM(void);
void setupTimer1(void);

// === 中斷服務函式 (ISR) ===
// 所有啟用的中斷來源都會進到這裡，由程式自行判斷是哪一個中斷
void __interrupt() ISR(void) {
    // 判斷是不是 Timer1 溢位中斷
    if (PIR1bits.TMR1IF) {       // TMR1IF = 1 表示 Timer1 overflow 了一次
        PIR1bits.TMR1IF = 0;     // 清除 Timer1 中斷旗標，否則會一直進這裡

        // 重新載入 Timer1 計數初值
        // Timer1 是 16-bit 計數器，TMR1H:MSB、TMR1L:LSB
        // 這裡載入 0xFF62 → 離 0xFFFF 還有 0x009D (157) 個 tick 就 overflow
        // tick 時間取決於 Fosc 和預分頻，目前大約是幾十毫秒一次中斷
        TMR1H = 0xFF;
        TMR1L = 0x62;

        // 如果 running = 1，才更新伺服位置與 LED
        if (running) {
            // 到達邊界就反向：
            // ccpr <= 15 時代表「最左」，往右 (sign = 1)
            // ccpr >= 75 時代表「最右」，往左 (sign = 0)
            if (ccpr >= 75) sign = 0;
            if (ccpr <= 15) sign = 1;

            // 根據方向決定 ccpr 要加還是減
            if (sign) ccpr++;    // sign = 1 → 往右：ccpr++
            else      ccpr--;    // sign = 0 → 往左：ccpr--

            // 依據新的 ccpr 值更新 PWM duty，實際改變伺服角度
            setServoAngle(ccpr);

            // LED 亮度同步變化：ccpr 從 15~75 對應亮度 0~255 (8-bit)
            // (ccpr - 15) → 0~60 之間，乘 255 / 60 變成 0~255
            unsigned char brightness = (ccpr - 15) * 255 / (75 - 15); // 0~255

            // 用 CCP2 來做 LED 的 PWM 輸出
            // 10-bit PWM duty 值分成：高 8-bit 在 CCPR2L，低 2-bit 在 CCP2CON<5:4>
            CCPR2L = brightness >> 2;          // 高 8 bits
            CCP2CONbits.DC2B = brightness & 0b11; // 低 2 bits
        }
    }
}

// === 主程式 ===
void main(void) {
    // === 時脈設定 ===
    // IRCF = 0b001 → Fosc = 125 kHz（內部 RC 振盪器）
    // 這裡註解寫 4 MHz 是錯的，要以 IRCF 為準
    OSCCONbits.IRCF = 0b001;  // Internal OSC = 125 kHz
    // OSCCONbits.SCS 預設使用主系統時脈，這裡不特別改也可以

    // === I/O 方向設定 ===
    TRISCbits.TRISC2 = 0;  // RC2 為輸出腳 → 接 CCP1 (伺服訊號)
    TRISCbits.TRISC1 = 0;  // RC1 為輸出腳 → 接 CCP2 (LED PWM)
    TRISBbits.TRISB0 = 1;  // RB0 為輸入腳 → 接按鈕

    // 如果你有接上拉電阻，可以打開內部上拉：
    // INTCON2bits.RBPU = 0;   // 啟用 PORTB 全域上拉控制
    // WPUB = 0x01;            // 啟用 RB0 的個別上拉

    setupPWM();      // 設定 PWM (Timer2 + CCP1/2)
    setupTimer1();   // 設定 Timer1 產生週期性中斷

    // 一開始先把伺服移到起始位置 (ccpr = 15 → 最左)
    setServoAngle(ccpr);

    // === 主迴圈：只負責讀按鈕，切換 running 狀態 ===
    while (1) {
        if (PORTBbits.RB0 == 0) {     // RB0 為 0 → 按鈕被按下（假設接地觸發）
            __delay_ms(20);           // 簡單消抖 (debounce)，等抖動結束
            // 等待按鈕放開，避免長按觸發多次
            while (PORTBbits.RB0 == 0);
            running = !running;   // 切換運動狀態：0→1 或 1→0
            __delay_ms(20);       // 放開後再消抖一次
        }
    }
}

// === Servo PWM 設定 ===
void setupPWM(void) {
    // === CCP1 -> 伺服馬達 ===
    // 使用 Timer2 產生 PWM 週期
    PR2 = 0x9B;                // 設定 PWM 週期：配合 Fosc=125k → 約 20ms (伺服標準)
    T2CONbits.T2CKPS = 0b01;   // Timer2 預分頻 1:4
    CCP1CONbits.CCP1M = 0b1100;// CCP1 設為 PWM 模式
    TRISCbits.TRISC2 = 0;      // RC2 設為輸出
    T2CONbits.TMR2ON = 1;      // 啟動 Timer2，開始產生 PWM 時基

    // === CCP2 -> LED 亮度控制 ===
    CCP2CONbits.CCP2M = 0b1100;// CCP2 也設為 PWM 模式
    TRISCbits.TRISC1 = 0;      // RC1 輸出
    CCPR2L = 0;                // 初始亮度設為 0（LED 暗）
}

// === Timer1 設定 ===
void setupTimer1(void) {
    // T1CON = 0b00110001 說明：
    // bit7-6: 未用
    // bit5-4: 預分頻 11b = 1:8
    // bit3  : 同步外部時鐘，這裡用內部，所以無所謂
    // bit2  : clock source，0 = Fosc/4，這裡實際是 0，但你沒特別設
    // bit1  : 16-bit/8-bit mode（這顆是 16-bit）
    // bit0  : Timer1 On = 1
    T1CON = 0b00110001;  // Timer1 on, 預分頻 1:8

    // 設定起始值 TMR1H:L = 0xFF62
    // 讓 Timer1 從這個值往上數到 0xFFFF 就 overflow → 觸發中斷
    TMR1H = 0xFF;        // 高 8 bits
    TMR1L = 0x62;        // 低 8 bits

    PIR1bits.TMR1IF = 0; // 清除 Timer1 中斷旗標
    PIE1bits.TMR1IE = 1; // 啟用 Timer1 中斷
    INTCONbits.PEIE = 1; // 啟用周邊中斷
    INTCONbits.GIE = 1;  // 啟用全域中斷
}

// === 設定伺服角度（實際上是設定 PWM duty） ===
// 這裡你沒有用「角度」，而是用 ccprValue 這個抽象數值 (15~75)
void setServoAngle(unsigned char ccprValue) {
    // 範圍保護：避免超出 15~75
    if (ccprValue >= 75) ccprValue = 75;
    if (ccprValue <= 15) ccprValue = 15;

    // CCP1 的 10-bit duty 值分成：
    //   高 8 bits → CCPR1L
    //   低 2 bits → CCP1CONbits.DC1B<1:0>
    //
    // 在你這裡，直接把 ccprValue 當成「10-bit duty」的低 8+2 bits（簡化版）
    CCPR1L = ccprValue >> 2;          // 取掉最低兩位，塞進 CCPR1L
    CCP1CONbits.DC1B = ccprValue & 0b11; // 取最低兩位，塞進 DC1B<1:0>
}
