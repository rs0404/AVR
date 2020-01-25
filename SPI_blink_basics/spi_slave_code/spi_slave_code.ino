/*-- Description: This is the code for slave atmega328p. We have used
                  Arduino as avr and arduino_ide for programming. 
                  The slave atmega328p receives the data sent from master.
                  If data received is 'y', LED connected PC0 or A0 pin of Arduino
                  is ON aswell as it transmit character '1' to the Serial Monitor.
                  And if 'n', LED is off and send '0' to Serial Monitor.
                  --*/
/*-- Date:        22nd January 2020--*/
/*-- Author:      Roshan Shrestah--*/

/*-- Preamble --*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VALUE (int)(F_CPU/(16UL*BAUD) - 1)     // For normal asynchronous mode


void UART_init() {
  UBRR0H = (UBRR_VALUE >> 8);
  UBRR0L = (UBRR_VALUE);
  UCSR0B = (1 << TXEN0) | (1 << RXEN0);                       // Enable the transmitter
  UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);          // 8 bit data mode
}


void UART_transmit(unsigned char data) {
  
  while(!(UCSR0A & (1 << UDRE0)));
  
  UDR0 = data;
}


unsigned char UART_receive() {
  
  while(!(UCSR0A & (1 << RXC0)));
  
  return UDR0;
}


  void SPI_slave_init() {
    // Set MISO output
    DDRB = (1 << PB4);   //PB4., digital pin 11 of arduino
    // Enable SPI
    SPCR = (1 << SPE);  
  }
  
  
  unsigned char SPI_receive(void) {
    
    // Wait until transmission complete
    while(!(SPSR & (1 << SPIF)));
    
    return (SPDR);
  }



int main(void) {
  SPI_slave_init();
  UART_init();
  
  //Set PC0 (A0 pin of arduino) as output to response for the received data
  DDRC = (1 << PC0);
  
  while(1) {
    
    if(SPI_receive() == 'y') {
      UART_transmit('1');
      PORTC |= (1 << PC0);
    }
    
    if (SPI_receive() == 'n') {
      UART_transmit('0');
      PORTC &= ~(1 << PC0);
    }
    
  }
  
  return 0;
}
