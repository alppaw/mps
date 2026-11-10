/*
 * main.c
 *
 * Created: 11/02/2026 11:58:37 PM
 *  Author: azat
 */ 

#include <xc.h>
#include <avr/interrupt.h>

#define F_CPU 8000000UL
#define MINUS_CODE 0x40

#define COUNT_MIN        (-17000)
#define COUNT_MAX        17000
#define COUNT_START      0
#define COUNT_PERIOD_MS  100

#define COUNT_UP         1
#define COUNT_DOWN       (-1)
#define COUNT_DIRECTION  COUNT_UP

#define COUNT_STEP  ((unsigned int)((F_CPU / 256UL) * COUNT_PERIOD_MS / 1000UL - 1))

#define DIGITS     8
#define DIG_LATCH  0
#define SEG_LATCH  1

unsigned char number[8] = {1, 2, 3, 4, 5, 6, 7, 8};

void delay_ms(unsigned int milliseconds)
{
    volatile unsigned int i;

    while (milliseconds > 0) {
        for (i = 0; i < (2000); i++) {
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

void timer_ini (unsigned int time_step)
{
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;

    OCR1A  = time_step;

    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS12);

    TIMSK  |= (1 << OCIE1A);
}

int div_mod_10 (int num, char *remainder)
{
    *remainder = num % 10;
    return num / 10;
}

int sign_swap (int num)
{
    return ~num + 1;
}

void output_formation (signed int input_number)
{
    unsigned char code[DIGITS];
    unsigned char i;
    int value;
    char rem;
    unsigned char negative = (input_number < 0);

    value = negative ? sign_swap(input_number) : input_number;

    for (i = 0; i < DIGITS; i++) code[i] = 0x00;

    i = 0;
    do {
        if (i >= DIGITS) break;
        value = div_mod_10(value, &rem);
        code[DIGITS - 1 - i] = converter(rem);
        i++;
    } while (value);

    if (negative && i < DIGITS)
        code[DIGITS - 1 - i] = MINUS_CODE;

    for (i = 0; i < DIGITS; i++) {
        write_to_latch(0x00, DIG_LATCH);
        write_to_latch(code[i], SEG_LATCH);
        write_to_latch(1 << i, DIG_LATCH);
        delay_ms(1);
    }
    write_to_latch(0x00, DIG_LATCH);
}

volatile signed int  counter   = COUNT_START;
volatile signed char count_dir = COUNT_DIRECTION;

ISR(TIMER1_COMPA_vect)
{
    if (count_dir > 0) {
        if (counter >= COUNT_MAX) counter = COUNT_MIN;
        else                      counter++;
    } else {
        if (counter <= COUNT_MIN) counter = COUNT_MAX;
        else                      counter--;
    }
}

int main(void)
{
    signed int shown;

    ports_init();
    timer_ini(COUNT_STEP);
    sei();

    while (1) {
        cli();
        shown = counter;
        sei();

        output_formation(shown);
    }
}