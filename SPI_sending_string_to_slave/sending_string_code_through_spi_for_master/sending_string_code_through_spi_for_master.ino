/*-- Description:
                 */
/*-- Date:        23 January 2020 --*/
/*-- Author:      Roshan Shrestha --*/

#include <avr/io.h>
#include <util/delay.h>

# define PD2 SS                                   // For Atmega328p
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


void UART_transmit_string( char str[]) {
   int i = 0;

   while(str[i] != '\0') {					// '\0' is null character which indicates the end of string.
      UART_transmit(str[i]);
      i ++;
   }

}


unsigned char UART_receive() {
  
  while(!(UCSR0A & (1 << RXC0)));        // Wait untile RXC0 bit is cleared ie. previously sent 
                                         //date is completely received
  return UDR0;
}


void UART_receive_string(char * string, uint8_t max_length) {				
  char response;
  uint8_t i = 0;

  while (i < (max_length - 1)) {	 // Last index is required to store null character
      response = UART_receive();
      //UART_transmit(response);

      if (response == 32) break;      

      else {
	 string[i] = response;
	 i++;
      }

   }
   
   // This line is necessary to indicate the end of string otherwise random character 
   // gets inserted till the string size reaches the maximum size provided.
   string[i] = '\0';
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


void SPI_transmit_string(char string_to_be_sent[]) {
    int index_count = 0;
    
    // Loop until end of string is reached
    while(string_to_be_sent[index_count] != '\0') {
      SPI_transmit(string_to_be_sent[index_count]);
      index_count ++;
    }
    
}


int main(void) {
  SPI_master_init();
  UART_init();
  //unsigned char data;
  
  while(1) {
    //data = UART_receive();
    //SPI_transmit(data);
    UART_transmit_string("Starting\n");
    
    SPI_transmit_string("Hello");
    UART_transmit_string("Ending\n");
  }
  
  return 0;
}
  
  
