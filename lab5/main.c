#include <xc.h>
#pragma config WDT = OFF

extern unsigned char is_prime(unsigned char n);
extern unsigned int count_primes(unsigned int n, unsigned int m);
extern long mul_extended(int n, int m);
extern long mul_extended_hardware(int n, int m);

void main(void){
//    volatile unsigned char ans = is_prime(107);
    volatile unsigned int ans = count_primes(107, 256);
//    volatile long ans = mul_extended(12345, 23456);
//    volatile long ans = mul_extended_hardware(12345, 23456);
    while(1);
    return;
}
