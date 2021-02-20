/*-- Description : Toggle LED at 184ms delay using timer0 with  prescale 1024.
 * F_CPU frequency is 1MHz which makes timer frequency equals 976.5625
 * For the delay of 184ms timer count equals req delay*time period
 * 	i.e. .184*976.5625 = 179.69 ~ 180 counts
 * After 180 counts the timer should overflow for which TCNT0 value should be
 * equal to 256 - 180 = 76 
 * WITHOUT INTERRUPT
 * Date : 29th May 2020 
 *--*/

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

void timer0_int() {
	TCCR0 = (1 << CS02) | (1 << CS00);	// Prescale of 1024
	TCNT0 = 76;	// As calculated in Descrition
}


int main(void) {
	DDRB = (1 << PB0);	// Set PBO to OUTPUT
	timer0_int();

	whie(1) {

		if (TIFR & TOV0) {
			PORTB ^= (1 << PB0);	// Toggle PB0
			TCNT0 = 76;	
				// Again start the connt from 76 rather than 0
			TIFR |= (1 << TOV0);	// Clear the overflow flag 
		}

	}

	return 0;
}
