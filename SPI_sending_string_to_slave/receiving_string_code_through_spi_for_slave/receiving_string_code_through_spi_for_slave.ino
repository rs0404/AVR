/*-- Description: Code for slave to receive the string sent by master through SPI communication.
                  UART also involved in this case.
                  --*/
/*-- Date:        27 January 2020 --*/
/*-- Author:      Roshan Shrestha --*/

#include <avr/io.h>
#include <util/delay.h>

/*-- For UART communication--*/
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

   while(str[i] != '\0') {		// '\0' is null character which indicates the end of string.
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
      
      // To indicate the end of string when Space is encountered, Arduino IDE serial monitor can't sent 
      // carriage return or Enter as character.
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
  

void SPI_receive_string(char * string, uint8_t max_length) {				
  char response;
  uint8_t i = 0;

  while (i < (max_length - 1)) {	 // Last index is required to store null character.
      response = SPI_receive();
      
      // To indicate the end of string when Space is encountered, Arduino IDE serial monitor can't sent carriage return.
      // or Enter as character.
      if (response == 32) break;      // 32 is the ASCII value of space.

      else {
	 string[i] = response;
	 i++;
      }

   }
   
   // This line is necessary to indicate the end of string otherwise random character 
   // gets inserted till the string size reaches the maximum size provided.
   string[i] = '\0';
}  


int main(void) {
  SPI_slave_init();
  UART_init();
  //unsigned char data;
  char str[30];
  
  while(1) {
    UART_transmit_string("Start\n");   // To check the beginning of program.
    SPI_receive_string(str, 30); 
    UART_transmit_string(str);
    UART_transmit_string("\nEnd\n");   // To check that a string is received through SPI communication.
    
  }
  
  return 0;
}
  
  
