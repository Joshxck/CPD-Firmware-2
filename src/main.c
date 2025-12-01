#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define EN_PIN   PB0
#define STEP_PIN PB1
#define DIR_PIN  PB2

volatile uint32_t ms_ticks = 0;
volatile uint16_t next_step_time = 0;
volatile uint16_t step_interval_ms = 1;

volatile uint16_t next_dir_time = 0;
volatile uint16_t dir_time_interval = 5000;

ISR(TIMER1_COMPA_vect)
{
    ms_ticks++;

    // Direction flip
    if (ms_ticks >= next_dir_time) {
        next_dir_time = ms_ticks + dir_time_interval;
        PORTB ^= (1<<DIR_PIN);
    }

    // Step pulse
    if (ms_ticks >= next_step_time) {
        next_step_time = ms_ticks + step_interval_ms;

        PORTB |=  (1<<STEP_PIN);
        _delay_us(3);
        PORTB &= ~(1<<STEP_PIN);
    }
}

void setup_timer_interrupt(void) {
    // CTC mode
    TCCR1A = 0;
    TCCR1B = (1 << WGM12);

    // 1ms compare match
    OCR1A = 249;

    // Interrupt
    TIMSK1 = (1 << OCIE1A);

    // prescaler 64
    TCCR1B |= (1 << CS11) | (1 << CS10);
}

int main(void)
{
    DDRB |= (1<<EN_PIN) | (1<<STEP_PIN) | (1<<DIR_PIN);

    // Enable driver: EN = LOW
    PORTB &= ~(1<<EN_PIN);

    setup_timer_interrupt();
    sei();

    while (1) {}
}
