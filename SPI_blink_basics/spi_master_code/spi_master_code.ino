/*-- Description:  This is the code for master atmega328p. We have used
                   Arduino as avr and arduino_ide for programming. 
                   The master receives the 8 bit data or character from UART communication 
                   where data is sent through computer keyboard to the master and then
                   sent to slave through SPI communication.
                   
                   --*/
/*-- Date:        22nd January 2020--*/
/*-- Author:     Roshan Shrestha--*/

/*-- Preamble --*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VALUE (int)(F_CPU/(16UL*BAUD) - 1)   // For normal asynchronous mode


void UART_init() {
  UBRR0H = (UBRR_VALUE >> 8);
  UBRR0L = (UBRR_VALUE);
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);    // Enable the receiver
  UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);  // 8 bit data 
}


void UART_transmit(unsigned char data) {
  while(!(UCSR0A & (1 << UDRE0)));        // Wait until the UDR0 buffer is empty
  UDR0 = data;
}


unsigned char UART_receive() {
  while(!(UCSR0A & (1 << RXC0)));        // Wait untile RXC0 bit is cleared ie. previously sent 
                                         //date is completely received
  return UDR0;
}


void SPI_master_init() {
  // Set MOSI and SCK output
  DDRB = (1 << PB3) | (1 << PB5);       // PB3 is MOSI, digital pin 11 of arduin and PB5 pin 13 
  // Set PB2, digital pin 8 as output to use as slave select pin. This pin when set as output 
  // function as normal GPIO pin
  DDRB |= (1 << PB2);
  // Enable SPI, Master, set clock rate fck/16
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}


void SPI_transmit(unsigned char data) {
  // Enable Slave 
  PORTB &= ~(1 << PB2);
  // Load data into the buffer
  SPDR = data;
  
  // Wait until transmission complete
  while(!(SPSR & (1 << SPIF)));
  
  // Disable Slave
  PORTB |= (1 << PB2);
  
}


int main(void) {
  SPI_master_init();
  UART_init();
  
  unsigned char data;                  // To store data received from UART communication
  
  while(1) {
    data = UART_receive();
    SPI_transmit(data);
  }
  
  return 0;
}
  
  
