#include <avr/io.h>
#include "characterLCD.h"

int main () {
  // characterLCD lcd(cmd_prt, data_prt, rs, en, D4, D5, D6, D7);
  // four-bit mode, rw conncected to ground
  characterLCD lcd(&PORTB, &PORTD, 0, 1, 2, 4, 5, 6, 7);
  lcd.begin(16, 2);

  while(1) {
    lcd.setCursor(0, 1);
    lcd.print("Hello!");
    lcd.setCursor(4,0);
    lcd.print(" RS");  
  }

  return 0;
}
