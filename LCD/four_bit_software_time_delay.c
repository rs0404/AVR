/*-- Description: Using LCD in 4-bit mode using single port only.
 *   This uses time delay instead of Busy Flag(BF).
 *   Note that the delay required is given in data sheet and delay used here is 
 *   considering the worst condition like using clock frequency 190 KHz 
 *   --*/
/*-- Date: 19 April 2020 --*/


#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 1000000UL	// CPU frequency set to internal 1MHz


/*-- Definition Section --*/
#define LCD_DDR DDRA	// Both command and data DDR (D4-D7 pins of LCD to PORTA.4-PORTA7)
#define LCD_PRT PORTA	// Both command and data PORT
#define LCD_RS  0	// Connected to PA0
#define LCD_RW  1	// Connected to PA1
#define LCD_EN  2	// Connected to PA2


/*-- LCD Instructions --*/
#define LCD_CLEAR	   0b00000001	// Replace all characteres with ASCII 'space'
#define LCD_HOME	   0b00000010	// Return cursor to first position on first line
#define LCD_ENTRY_MODE	   0b00000110	// Shift cursor from left to right on read/write
#define LCD_DISPLAY_OFF	   0b00001000	// Turn display off
#define LCD_DISPLAY_ON	   0b00001100	// Display on, cursor off, don't blink character
#define LCD_FUNCTION_RESET 0b00110000	// Reset the LCD
#define LCD_FUNCTION_4BIT  0b00101000	// 4-bit data, 2-line display, 5*7 font


/*-- User-defined Functions Declaration Section --*/
void lcd_command(unsigned char cmnd);
void lcd_data(unsigned char data);
void lcd_gotoxy(unsigned char x, unsigned char y);
void lcd_init();
void lcd_print(char *str);
void lcd_write_4(unsigned char byte_value);
void delay_ms(unsigned int delay);	// Why made my own delay function?
	// For this see the comment of the code for 8-Bit mode.
void delay_us(unsigned int delay);


/*-- Main Function Section --*/
int main (void) {
	lcd_init();
	while(1) {
		lcd_gotoxy(1,1);
		lcd_print("Wow");
		lcd_gotoxy(5,2);
		lcd_print("Level up");
		delay_ms(2000);
		lcd_gotoxy(1,1);
		lcd_print("Few more to go");
		delay_ms(2000);
		// When you observe LCD display after sometime you only see change in "Few"
		// and "Wow". It is because only the first three location of DDRAM is changed
		// and other remains same. You can use LCD_CLEAR command to see your desired
		// result.
	}
	return 0;
}


/*-- Function Definition Section --*/
void lcd_command(unsigned char cmnd) {
	LCD_PRT &= ~(1 << LCD_RS);	// RS = 0 for command
	LCD_PRT &= ~(1 << LCD_RW);      // RW = 0 for write mode
	LCD_PRT &= ~(1 << LCD_EN);	// Make sure E is initially low
	lcd_write_4(cmnd);		// Write the upper 4 bits of command
	lcd_write_4(cmnd << 4);		// Write the lower 4 bits of command
}

void lcd_data(unsigned char data) {
	LCD_PRT |= (1 << LCD_RS);
	LCD_PRT &= ~(1 << LCD_RW);
	lcd_write_4(data);				// Write the upper 4 bits of data
	lcd_write_4(data << 4);				// Write the lower 4 bits of data
}

void lcd_write_4(unsigned char byte_value) {
	// In 4-bit mode we have to sent the 4 bit of data only from initial although the LCD
	// is initially at 8-bit mode. If we use lcd_command function initially then it will
	// breaks the 8-bit into two 4-bit and send to the LCD. But  the reset
	// function's lower four bits are irrelevant 
	LCD_PRT &= 0x0F;// Data in each data pin of LCD be 0
	LCD_PRT |= (byte_value & 0xf0);			// Set data pins according to cmnd's
		// upper nibble
// write the data
	LCD_PRT |= (1<<LCD_EN);				
	delay_us(1);					// Implement 'Data set-up time' (80 
		// ns) and 'Enable pulse'
	LCD_PRT &= ~(1<<LCD_EN);			
	delay_us(1);					// Implement 'Data hold time' (10 ns) 
		// and 'Enable cycle'
}	

void lcd_init() {
	delay_ms(100);					// Wait for stable power
	LCD_DDR = 0xFF;					// LCD port as output
	LCD_PRT &= ~(1 << LCD_EN);			// Make sue E is initially low
	lcd_write_4(LCD_FUNCTION_RESET);			// First part of reset sequence
	delay_ms(10);					// 4.1 ms delay(min);
	lcd_write_4(LCD_FUNCTION_RESET);			// Second part of reset sequence
	delay_us(200);					// 100us delay(min)
	lcd_write_4(LCD_FUNCTION_RESET);			// Third part of reset sequence
	delay_us(200);					// This delay is omitted in the data sheet
	lcd_write_4(LCD_FUNCTION_4BIT);			// Set 4-bit mode
	delay_us(80);					// 40 us delay (min)

	lcd_command(LCD_FUNCTION_4BIT);			// Set mode, lines, and font
	delay_us(80);					// 40 us delay (min)
// The next three instruction are specified in the data sheet as part of initialization routine,
// so it is a good idea (but not necessary) to do them just as specified and then redo them later
// if the application requires  a different configuration.
	lcd_command(LCD_DISPLAY_OFF);			// Turn display OFF
	delay_us(80);
	lcd_command(LCD_CLEAR);				// Clear Display Data RAM
	delay_us(80);
	lcd_command(LCD_ENTRY_MODE);			// Set the desired shift characteristics
	delay_us(80);
	lcd_command(LCD_DISPLAY_ON);			// Turn the display on
	delay_us(80);	
}

void lcd_gotoxy(unsigned char x, unsigned char y) {	// First column then row
	unsigned char first_char_adr[] = {0x80, 0xC0, 0x94, 0xD4};
		// These are the starting address of each row in LCD as can be 
		// seen from data sheet
	lcd_command(first_char_adr[y-1] + x-1);		// To move the cursor to
		// the defined location the address of that location should be
		// send as command for eg. the command for 2nd column 1st row is
		// 0x81.
	delay_us(100);
}

void lcd_print(char *str) {
	unsigned char i = 0;
	while(str[i] != 0) { 				
		// 0 is the ACII value for null character 
		lcd_data(str[i]);
		i++;
		delay_us(80);				// 40 us delay (min)
	}
}

void delay_ms(unsigned int delay) {
	//_delay_ms(d);
	//*-- This line gives us error because we cannot use the delay function
	//    with the variable value as given in the user manual --* //
	while(delay --) {
    		_delay_ms(1);
	}
}

void delay_us(unsigned int delay) {
	while(delay --) 
    		_delay_us(1);	
}
	
