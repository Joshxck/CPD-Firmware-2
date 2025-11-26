#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>

#define MASK(x) ((unsigned char) (1<<x))

#define EN_PIN PB0
#define STEP_PIN PB1
#define DIR_PIN PB2

volatile uint32_t ms_ticks = 0;
volatile uint8_t step_index = 0;
volatile uint16_t next_step_time = 0;   // when to advance motor
volatile int8_t step_dir = 1;           // +1 or -1
volatile uint16_t step_interval_ms = 10; // speed (1 step every 5ms)

volatile uint16_t next_dir_time = 0;
volatile uint16_t dir_time_interval = 2000;

ISR(TIMER1_COMPA_vect)
{
    ms_ticks++;

    if (ms_ticks >= next_dir_time) {
        next_dir_time = ms_ticks + dir_time_interval;
        PORTB ^= (1<<DIR_PIN);
    }

    if (ms_ticks >= next_step_time) {
        next_step_time = ms_ticks + step_interval_ms;
        PORTB ^= (1<<STEP_PIN);
    }
}

void setup_timer_interrupt(void) {
    // ----- Timer1 Setup -----
    // CTC mode: clear timer on compare
    TCCR1B |= (1 << WGM12);

    // Compare value for 1ms
    OCR1A = 249;

    // Enable interrupt on compare match A
    TIMSK1 |= (1 << OCIE1A);

    // Start timer with prescaler 64
    TCCR1B |= (1 << CS11) | (1 << CS10);
}

int main(void)
{
    DDRB |= (1<<EN_PIN) | (1<<STEP_PIN) | (1<<DIR_PIN); // Set PB0-0 as output (digital pins 8 - 11)
    PORTB &= ~((1<<EN_PIN) | (1<<DIR_PIN)); // Turn digital pins 8, 10 OFF

    setup_timer_interrupt();
    sei();

    while (1) {}
}


