/*
 * main.c
 *
 * Created: 10/19/2026 10:58:37 PM
 *  Author: aleks
 */ 

#include <xc.h>

#define F_CPU 8000000UL



unsigned char number[8] = {1, 2, 3, 4, 5, 6, 7, 8};

void delay_ms(unsigned int milliseconds)
{
    volatile unsigned int i;

    while (milliseconds > 0) {
        for (i = 0; i < 2000; i++) {
        }

        milliseconds--;
    }
}

unsigned char converter(unsigned int a)
{
    switch (a) {
        case 0:  return 0x3F;
        case 1:  return 0x06;
        case 2:  return 0x5B;
        case 3:  return 0x4F;
        case 4:  return 0x66;
        case 5:  return 0x6D;
        case 6:  return 0x7D;
        case 7:  return 0x07;
        case 8:  return 0x7F;
        case 9:  return 0x6F;
        case 10: return 0x77; 
        case 11: return 0x7C; 
        case 12: return 0x39; 
        case 13: return 0x5E; 
        case 14: return 0x79; 
        case 15: return 0x71;
        default: return 0x00;
    }
}

void out_to_reg(char a, int dest, char le)
{
    if (dest == 0) {
        PORTA = a;

        if (le == 1) {
            PORTF = PORTF | (1 << PF2);
        } else {
            PORTF = PORTF & ~(1 << PF2);
        }
    } else {
        PORTC = a;

        if (le == 1) {
            PORTF = PORTF | (1 << PF1);
        } else {
            PORTF = PORTF & ~(1 << PF1);
        }
    }
}

void write_to_latch(unsigned char data, int dest)
{
    out_to_reg(data, dest, 1);
    out_to_reg(data, dest, 0);
}

void ports_init(void)
{
    DDRA = 0xFF;
    DDRC = 0xFF;

    DDRF = DDRF | (1 << PF1) | (1 << PF2);

    PORTA = 0x00;
    PORTC = 0x00;
    PORTF = PORTF & ~(1 << PF1);
    PORTF = PORTF & ~(1 << PF2);

    write_to_latch(0x00, 0);
    write_to_latch(0x00, 1);
}

void task1(void)
{
    write_to_latch(converter(8), 1);
    write_to_latch(0xFF, 0);
}

void task2(void)
{
    unsigned char digit;

    for (digit = 0; digit < 8; digit++) {
        write_to_latch(0x00, 0);
        write_to_latch(converter(8), 1);
        write_to_latch(1 << digit, 0);
        delay_ms(250);
    }
}

void task3(void)
{
    unsigned char digit;

    for (digit = 0; digit < 8; digit++) {
        write_to_latch(0x00, 0);

        write_to_latch(converter(number[digit]), 1);

        write_to_latch(1 << digit, 0);

        delay_ms(1);
    }
}

int main(void)
{
    unsigned char task = 3;

    ports_init();

    while (1) {
        if (task == 1) {
            task1();
        }

        if (task == 2) {
            task2();
        }

        if (task == 3) {
            task3();
        }
    }
}