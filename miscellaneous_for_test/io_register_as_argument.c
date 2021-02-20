/*-- Description: Passing I/O registers as argument of the function 
 * like PORTD --*/
 
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>

void port_test(volatile uint8_t *port) {
	// volatile compulsary to make change in the port appear each time.
	// Or see geekforgeek volatile article for more detial.
	// Address of the IO register should be passed so pointer is used.
	volatile uint8_t *_port = port;
	volatile uint8_t *ddr = (_port - 1);
	volatile uint8_t *pin = (_port - 2);
	*ddr = (1 << 1) | (1 << 3);	// Set output mode
	*port |= (1 << 3) & ~(1 << 1);	// Pass HIGH
	_delay_ms(1000);
	*port &= ~(1 << 3);	// Pass LOW
	_delay_ms(1000);
}

int main() {
	//uint8_t PORTD;	// Calling method	
	while(1) {
		port_test(&PORTD);	
	}

	return 0;

}
