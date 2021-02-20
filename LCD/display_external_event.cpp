/*-- Description: This is a demo program which displays the external event count
 * in the LCD. As an external event when the PA0 pin is grounded it behaves as 
 * if external event has ocurred. External event can be a man entering through
 * a door or like that.
 * To be specific this is method of displaying integer to LCD.
 *--*/

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "characterLCD.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  char buffer[4];	// To store numbers up to 3 digits and last character in 
    // buffer is null character.
  unsigned char i = 0;	// To count the external events
  characterLCD lcd(&PORTB, &PORTD, 0, 1, 2, 4, 5, 6, 7);
  lcd.begin(16,2);
  lcd.setCursor(0,1);
  lcd.print("Count event:");
  DDRA &= ~(1 << PA7);	//  Set for input to response to external event
  PORTA |= (1 << PA7);  // Enable internal pull-up
  lcd.setCursor(5,0);
  itoa(i,buffer,10);	// itoa(int value, char *str, int base) is define under
  	// stdlib.h header. It is one of the type casting functions in c like atof(), 
	// atoi(), ltoa() etc. It converts integer to string and adds null character 
	// at the end.
  //sprintf(buffer,"%d",i);	// Instead of itoa(), we can use sprintf() function
  	// to convert to any data type to string. This function is similar to printf()
	// function in C except it takes an additional argument at the beginning and 
	// instead of displaying it to console output it store in buffer without 
	// displaying. The first argument to this function is a pointer or character 
	// array to store the string in the buffer.
	
  lcd.print(buffer);

  while(1) {
    if(!(PINA & (1 << 7))) {
      lcd.setCursor(5,0);
      sprintf(buffer,"%d",++i);
      lcd.print(buffer); 
      while (!(PINA & (1 << 7))); 
      _delay_ms(99);

    }
    
  }
  return 0;
}
