/*-- Description: Code for slave to receive the string sent by master through SPI communication.
                  UART also involved in this case.
                  --*/
/*-- Date:        27 January 2020 --*/
/*-- Author:      Roshan Shrestha --*/

#include <avr/io.h>
#include <util/delay.h>

/*-- For UART communication--*/
#define F_CPU 8000000UL
#define BAUD 9600
#define UBRR_VALUE (int)(F_CPU/(16UL*BAUD) - 1)   // For normal asynchronous mode


void UART_init() {
  UBRRH = (UBRR_VALUE >> 8);
  UBRRL = (UBRR_VALUE);
  UCSRB = (1 << RXEN) | (1 << TXEN);    // Enable the receiver
  UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);  // 8 bit data 
}


void UART_transmit(unsigned char data) {
  while(!(UCSRA & (1 << UDRE)));        // Wait until the UDR buffer is empty
  UDR = data;
}


void UART_transmit_string( char str[]) {
   int i = 0;

   while(str[i] != '\0') {		// '\0' is null character which indicates the end of string.
      UART_transmit(str[i]);
      i ++;
   }

}


unsigned char UART_receive() {
  
  while(!(UCSRA & (1 << RXC)));        // Wait untile RXC bit is cleared ie. previously sent 
                                         //date is completely received
  return UDR;
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

int main () {
  char str[30];
  
  while(1) {
    UART_transmit('b');
    UART_receive_string(str, 30);
    UART_transmit_string(str);
  }
  
  return 0;
}
