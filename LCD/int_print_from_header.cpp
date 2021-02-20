/*-- Description: Display integer value in LCD using header file. --*/

#define F_CPU 1000000UL

#include "characterLCD.h"
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
  characterLCD lcd(&PORTB, &PORTD, 0, 1, 2, 4, 5, 6, 7);
  lcd.begin(16, 2);
  int i = 0;
  lcd.setCursor(0, 0);
  lcd.print(i);
  lcd.print("Hello");
  while(1)
  {
    lcd.setCursor(0,0);
    lcd.print(++i);
    _delay_ms(500);
  }

  return 0;
}
