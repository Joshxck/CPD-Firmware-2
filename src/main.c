#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

#define MASK(x) ((unsigned char) (1<<x))

#define A1 PB0
#define A2 PB1
#define B1 PB2
#define B2 PB3

volatile uint32_t ms_ticks = 0;
volatile uint8_t step_index = 0;
volatile uint16_t next_step_time = 0;   // when to advance motor
volatile int8_t step_dir = 1;           // +1 or -1
volatile uint16_t step_interval_ms = 5; // speed (1 step every 5ms)

volatile uint16_t next_dir_time = 0;
volatile uint16_t dir_time_interval = 2000;

static const char step_values_full[4] = {
    0b0000101,
    0b0000110,
    0b0001010,
    0b0001001
};

static const char step_values_half[8] = {
    0b0000101,
    0b0000100,
    0b0000110,
    0b0000010,
    0b0001010,
    0b0001000,
    0b0001001,
    0b0000001,
};

ISR(TIMER1_COMPA_vect)
{
    ms_ticks++;

    if (ms_ticks >= next_dir_time) {
        next_dir_time = ms_ticks + dir_time_interval;
        step_dir = -step_dir;
    }

    if (ms_ticks >= next_step_time) {
        next_step_time = ms_ticks + step_interval_ms;

        // advance the step index
        step_index = (step_index + step_dir) & 0x07;  // wrap 0–3
    }
}

void apply_step(uint8_t s)
{
    PORTB = step_values_half[s];
}

int main(void)
{
    DDRB |= (1<<A1) | (1<<A2) | (1<<B1) | (1<<B2); // Set PB0-3 as output (digital pins 8 - 11)
    PORTB &= ~((1<<A1) | (1<<A2) | (1<<B1) | (1<<B2)); // Turn digital pins 8 - 11 OFF

    // ----- Timer1 Setup -----

    // CTC mode: clear timer on compare
    TCCR1B |= (1 << WGM12);

    // Compare value for 1ms
    OCR1A = 249;

    // Enable interrupt on compare match A
    TIMSK1 |= (1 << OCIE1A);

    // Start timer with prescaler 64
    TCCR1B |= (1 << CS11) | (1 << CS10);

    sei();

    while (1) {
        apply_step(step_index);
    }
}
