/*-- C Program for LCD display --*/

/*-- Using 8-bit mode --*/



#include <avr/io.h>

#include <util/delay.h>



/*-- Definition Section --*/

#define LCD_DPRT PORTD	// Data PORT (For pins D0 - D7 of LCD)

#define LCD_CPRT PORTB	// Command PORT (For RS, R/W and E pins of LCD)

#define LCD_DDDR DDRD	// Data DDR

#define LCD_CDDR DDRB	// Command DDR

#define LCD_DPIN PIND	// Data PIN

#define LCD_CPIN PINB	// Command PIN

#define LCD_RS	 0	// Connected to PB0

#define LCD_RW	 1	// Connected to PB1

#define LCD_EN	 2	// Connected to PB2

unsigned char _data_pins[] = {0, 1, 2, 3, 4, 5, 6, 7};


/*-- User-defined function declaration section --*/

void delay_us(unsigned int d);	// Although predefined function for 

	// delay is available we made out own because different compiler

	// have their own delay functions like _delay_us() for WinAVR, 

	// delay_ms() for CodeVision and shows error while using diff

	// compiler if used predefined (not necessary).

void lcd_command(unsigned char cmd);

void lcd_data(unsigned char data);

void lcd_init();

void lcd_print (unsigned char *str);

void lcd_gotoxy(unsigned char x, unsigned char y);

void custom_char(); 

void write8bits(uint8_t value);





/*-- Start of main function --*/

int main (void) {

  unsigned char i;

  lcd_init();

  lcd_gotoxy(1,1);	// Printing starts from column 1, row 1 of LCD

  lcd_print("Hurray !"); // You can pass any string like "Hello World!"

  	// which you are used to in programming as there is believe that

	// luck comes in your favour starting with this. But I've used

	// "Hurray!" bc it's your expression when you see that being

	// printed on the LCD.



  while(1)

	 // This time I've kept while loop empty but if you want your

  	// LCD to change frequently you can write your code inside while

	// loop.

  return 0;

}





/*-- User-defined function definition section --*/

void delay_us(unsigned int delay) {

  while(delay --) 

    _delay_us(1);	// Note I'm not passing delay variable 

		// inside the _delay_us() function because this function

		// does not take the variable as parameter and should be

		// used with only constant value like 1.

}




void write8bits(uint8_t value) {
  for (int i = 0; i < 8; i++) {
    if((value >> i) & 0x01)
      LCD_DPRT |= (1 << _data_pins[i]);
    else
      LCD_DPRT &= ~(1 << _data_pins[i]);
  }
}

void lcd_command(unsigned char cmnd) {

	// Function to send command. Note command is also data of 8 bit

	// sent through D0-D7 pins .
  LCD_DPRT = cmnd;
  //write8bits(cmnd);		// Make the command available at PORT

  LCD_CPRT &= ~(1 << LCD_RS);	// RS = 0 for cmd mode

  LCD_CPRT &= ~(1 << LCD_RW);	// RW = 0 for write operation

  LCD_CPRT |= (1 << LCD_EN);	// Sending high pulse to EN telling LCD

  	// to receive data available at the PORT

  delay_us(1);			// According to datasheet minimum pulse

 	// should be 450ns, making it of 1us should work fine for almost

	// LCDs.

  LCD_CPRT &= ~(1 << LCD_EN);	// Sending low pulse to  EN indicating

  	// LCD we are done sending data and LCD process the receive data

  	

  delay_us(50);			// Giving time for LCD to process the 

  	// data

}





void lcd_data(unsigned char data) {

  LCD_DPRT = data;		// Make the data available at the PORT

  LCD_CPRT |= (1 << LCD_RS);	// RS = 1 for data mode

  LCD_CPRT &= ~(1 << LCD_RW);	// RW = 0 for write operation

  LCD_CPRT |= (1 << LCD_EN);	// Sending high pulse to EN

  delay_us(1);			// Increasing the pulse width. (If you

 	// are confuse in the terms time period and pulse then remember

	// time period is the sum of on and off time whereas pulse 

	// width refers only on time.

  LCD_CPRT &= ~(1 << LCD_EN);	// Sending low pulse to  EN

  _delay_us(50);	

}





void lcd_init() {

  LCD_DDDR = 0xFF;		// Set for output mode

  LCD_CDDR = 0x0F;

  LCD_CPRT &= ~(1 << LCD_EN);	// LCD EN = 0

  _delay_us(2000);			// Wait for stable power

// After power-up you should wait for sometime before sending 

// initialization commands to the LCD. If the LCD initializer 

// function is not the first function in your code  you can

// omit this delay.
  lcd_command(0x30);
  delay_us(4500);
  lcd_command(0x30);
  delay_us(150);
  lcd_command(0x30);
  delay_us(150);

  lcd_command(0x38);		// init. LCD 2 line, 5*7 matrix

  lcd_command(0x0E);		// display on, cursor on (if F instead of E then

// cursor blinking ON as well

  lcd_command(0x01);		// Clear LCD

  delay_us(2000);		// Clear LCD cmnd requires delay of about

 	// 2ms (takes longer time than other commands)

}





void lcd_gotoxy(unsigned char x, unsigned char y) {

  // x and y are the value of row and column respectively which starts 

  // from 1. To make it start from 0 replace y-1 and x-1 with y and x.

  unsigned char first_char_adr[] = {0x80, 0xC0, 0x94, 0xD4};

				// These are the starting addresses of

				// the cursor of 4 rows

  lcd_command(first_char_adr[y-1] + x-1);

  delay_us(100);

}





void lcd_print (unsigned char *str) {

  unsigned char i = 0;

  while(str[i] != 0) {

    lcd_data(str[i]);

    i++;

  }

}
