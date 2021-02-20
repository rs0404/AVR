/*-- Description: Demo of Cylon robot's eye using 8 LED's --*/

#include <avr/io.h>
#include <util/delay.h>

#define LED_PORT PORTA
#define LED_DDR  DDRA

int main (void) {
  LED_DDR = 0xFF;
  
  while(1) {   
    LED_PORT = 0b00000001;
    _delay_ms(20);
    LED_PORT = 0b00000010;
    _delay_ms(20);   
    LED_PORT = 0b00000100;
    _delay_ms(20);
    LED_PORT = 0b00001000;
    _delay_ms(20);   
    LED_PORT = 0b00010000;
    _delay_ms(20);
    LED_PORT = 0b00100000;
    _delay_ms(20);
    LED_PORT = 0b01000000;
    _delay_ms(20);
    LED_PORT = 0b10000000;
    _delay_ms(20);
    
    LED_PORT = 0b10000000;
    _delay_ms(20);   
    LED_PORT = 0b01000000;
    _delay_ms(20);
    LED_PORT = 0b00100000;
    _delay_ms(20);   
    LED_PORT = 0b00010000;
    _delay_ms(20);
    LED_PORT = 0b00001000;
    _delay_ms(20);    
    LED_PORT = 0b00000100;
    _delay_ms(20);
    LED_PORT = 0b00000010;
    _delay_ms(20);    
    LED_PORT = 0b00000001;
    _delay_ms(20);
  }

  return 0;
}


  
