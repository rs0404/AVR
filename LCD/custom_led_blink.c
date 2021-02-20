/*-- Description: This is a simple example of custom character display which 
 * shows an IC connected to LED and a button and animation of LED being ON when
 * button pressed and OFF when released is tried to be shown although it doesn't
 * looks like that.
 * 	4-bit, delay using busy flag is used here.
 *   --*/
/*-- Date: 21 April 2020 --*/


#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 1000000UL	// CPU frequency set to internal 1MHz


/*-- Definition Section --*/
#define LCD_DDR DDRA	// Both command and data DDR (D4-D7 pins of LCD to PORTA.4-PORTA7)
#define LCD_PRT PORTA	// Both command and data PORT
#define LCD_PIN	PINA	// Both command and data PIN
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
#define CGRAM_ADDRESS	   0b01000000	// Starting address of CGRAM
#define DDRAM_ADDRESS	   0b10000000	// Starting address of DDRAM


/*-- Global Variable Declaration Section --*/
unsigned char ic_first[8] = {0x1F, 0x10, 0x1F, 0x09, 0x09, 0x09, 0x09, 0x09};
unsigned char ic_middle[8] = {0x1F, 0x00, 0x1F, 0x09, 0x09, 0x09, 0x00, 0x00};
unsigned char ic_last[8] = {0x1F, 0x01, 0x1F, 0x09, 0x09, 0x09, 0x00, 0x00};
unsigned char LED[8] = {0x0C, 0x0A, 0x19, 0x09, 0x19, 0x0A, 0x0C, 0x00};
unsigned char bright[8] = {0x04, 0x08, 0x10, 0x1E, 0x10, 0x08, 0x04, 0x00};
unsigned char button_on[8] = {0x00, 0x04, 0x0E, 0x1F, 0x11, 0x1F, 0x11, 0x00};
unsigned char button_off[8] = {0x04, 0x04, 0x0A, 0x1F, 0x11, 0x1F, 0x11, 0x00};
unsigned char led_connection[8] = {0x09, 0x09, 0x09, 0x08, 0x0F, 0x00, 0x00, 0x00};

/*-- User-defined Functions Declaration Section --*/
void lcd_command(unsigned char cmnd);
void lcd_data(unsigned char data);
void lcd_gotoxy(unsigned char x, unsigned char y);
void lcd_init(void);
void lcd_print(char *str);
void lcd_write_4(unsigned char byte_value);
void lcd_check_BF_4(void);
void delay_ms(unsigned int delay);	// Why made my own delay function?
	// For this see the comment of the code for 8-Bit mode.
void delay_us(unsigned int delay);
void store_single_char(unsigned char loc, unsigned char *binary_data);	// To
	// store single custom character in CGRAM.
void built_char(void);		// This function stores all the custom characters
	// in CGRAM

/*-- Main Function Section --*/
int main (void) {
	unsigned char i = 0, col = 11;		// These variables are made to
			// change the position of the custom characters 
			// relatively changing just this value.
	unsigned  col_copy = col;		// To save the col value as it its.
	lcd_init();
	built_char();

	lcd_gotoxy(1,1);
	lcd_print("Blink LED");
	lcd_gotoxy(1,2);
	lcd_print("using IC");

	lcd_gotoxy(col,1);

	for(; i < 7; i++) {
		if(i >= 3)
			lcd_gotoxy(col_copy++,2);
		lcd_data(i);
	}
	lcd_gotoxy(col+2,1);
	lcd_data(1);
	lcd_data(1);
	lcd_data(1);
	lcd_data(2);
		
	while(1) {
		lcd_gotoxy(col+2,2);
		lcd_data(' ');
		lcd_data(7);
		delay_ms(1000);
		
		lcd_gotoxy(col+2,2);
		lcd_data(5);
		lcd_data(6);
		delay_ms(1000);
	}
	return 0;
}


/*-- Function Definition Section --*/
void store_single_char(unsigned char loc, unsigned char *binary_data) {
	unsigned char j;
	if(loc<8) {
		lcd_check_BF_4();
		lcd_command(CGRAM_ADDRESS + (loc*8));	// Each character for 5*8 font is
			// of 8 bytes and starts from location 0x40
		for(j = 0; j < 8; j++) {
			lcd_check_BF_4();
			lcd_data(binary_data[j]);	// Store each byte of a character
				// in the CGRAM
		}
	}
}

void built_char(void) {
	store_single_char(0, ic_first);
	store_single_char(1, ic_middle);
	store_single_char(2, ic_last);
	store_single_char(3, led_connection);
	store_single_char(4, LED);
	store_single_char(5, bright);
	store_single_char(6, button_on);
	store_single_char(7, button_off);
}

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

	lcd_check_BF_4();				// Make sure LCD controller is ready
	lcd_command(LCD_FUNCTION_4BIT);			// Set mode, lines, and font
	//delay_us(80);
	//------- From this point on the busy flag is available ------------//
// The next three instruction are specified in the data sheet as part of initialization routine,
// so it is a good idea (but not necessary) to do them just as specified and then redo them later
// if the application requires  a different configuration.
	lcd_check_BF_4();				// Make sure LCD controller is ready
	lcd_command(LCD_DISPLAY_OFF);			// Turn display OFF
	lcd_check_BF_4();				// Make sure LCD controller is ready
	lcd_command(LCD_CLEAR);				// Clear Display Data RAM
	lcd_check_BF_4();				// Make sure LCD controller is ready
	lcd_command(LCD_ENTRY_MODE);			// Set the desired shift characteristics
	lcd_check_BF_4();				// Make sure LCD controller is ready
	lcd_command(LCD_DISPLAY_ON);			// Turn the display on
}

void lcd_gotoxy(unsigned char x, unsigned char y) {	// First column then row
	unsigned char first_char_adr[] = {0x80, 0xC0, 0x94, 0xD4};
		// These are the starting address of each row in LCD as can be 
		// seen from data sheet
	
	lcd_check_BF_4();				// Make sure LCD controller is ready
	lcd_command(first_char_adr[y-1] + x-1);		// To move the cursor to
		// the defined location the address of that location should be
		// send as command for eg. the command for 2nd column 1st row is
		// 0x81.
	
}

void lcd_print(char *str) {
	unsigned char i = 0;
	while(str[i] != 0) { 				
		// 0 is the ACII value for null character 
		lcd_check_BF_4();				// Make sure LCD controller is ready
		lcd_data(str[i]);
		i++;
	}
}

void lcd_check_BF_4(void) {
	uint8_t busy_flag;				// Busy flag 'mirror'
	LCD_DDR &= ~(1<<7);				// Set PA7 to input
	LCD_PRT &= ~(1<<LCD_RS);
	LCD_PRT |= (1<<LCD_RW);				// Read from LCD module

	do {
		busy_flag = 0;				// Initialize busy flag 'mirror'
		LCD_PRT |= (1<<LCD_EN);			
		delay_us(1);				// Implement 'Delay data time' (160 ns)
			// and 'Enable pulse width' (230 ns)
		busy_flag |= (LCD_PIN & (1<<7));	// Consider only 7th bit and mask remaining
		LCD_PRT &= ~(1<<LCD_EN);		 
		delay_us(1);				// Implement 'Address hold time (10 ns), 
			// 'Data hold time' (10 ns), and 'Enable cycle time' (500 ns)
	
	// Read and discard alternate nibbles (D3 information)
		LCD_PRT |= (1<<LCD_EN);	
		delay_us(1);				// Implement 'Delay data time' (160 ns)
			// and 'Enable pulse width' (230 ns)
		LCD_PRT &= ~(1<<LCD_EN);		 
		delay_us(1);				// Implement 'Address hold time (10 ns), 
			// 'Data hold time' (10 ns), and 'Enable cycle time' (500 ns)
	} while(busy_flag);
	
	LCD_PRT &= ~(1<<LCD_RW);			// Write to LCD module
	LCD_DDR |= (1<<7);				// Reset D7 data direction to output
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
	



