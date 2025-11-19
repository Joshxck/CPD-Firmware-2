#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRB |= (1 << DDB5);   // PB5 = output (digital 13)

    while (1) {
        PORTB |= (1 << PORTB5);   // LED ON
        _delay_ms(500);

        PORTB &= ~(1 << PORTB5);  // LED OFF
        _delay_ms(500);
    }
}