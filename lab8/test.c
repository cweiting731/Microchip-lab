#include <stdio.h>

// 15 ~ 75
int main() {
    unsigned char CCPR = 15; // 15 ~ 75 (500us ~ 2400us)
    unsigned char sign = 0;
    for (int i=0; i<90; i++) {
        if (CCPR >= 75) sign = 0;
        if (CCPR <= 15) sign = 1;
        if (sign) CCPR++;
        else CCPR--;
        // 將 CCPR 轉成10bits的binary數值 前8bits用hex顯示 後2bits用binary顯示
        unsigned char CCPR_high = CCPR >> 2;
        unsigned char CCPR_low = CCPR & 0b11;
        printf("CCPR: %d CCPR: %02X, DC1B: %02X\n", CCPR, CCPR_high, CCPR_low);
    }

}