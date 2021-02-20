/*-- Description: Simple interfacing of 4*4 keypad with avr and display the output to the character LCD --*/
/*-- Date: 12 April 2020 --*/

#include <avr/io.h>
#include <util/delay.h>

#define KEY_PRT PORTD		// Keyboard PORT
#define KEY_DDR DDRD		// Keyboard DDR
#define KEY_PIN PIND		// Keyboard PIN

#define LCD_DDR DDRA
#define LCD_PRT PORTA
#define LCD_RS  0
#define LCD_RW  1
#define LCD_EN  2

/*-- User-defined Functions Declaration Section --*/
void lcd_command(unsigned char cmnd);
void lcd_data(unsigned char data);
void lcd_gotoxy(unsigned char x, unsigned char y);
void lcd_init();
void lcd_print(char *str);
void delay_ms(unsigned int delay);

unsigned char keypad[4][4] = { '1', '2', '3', 'A',
			       '4', '5', '6', 'B',
			       '7', '8', '9', 'C',
			       '*', '0', '#', 'D'};

int main(void) {
	unsigned char colloc, rowloc;		// For row and column of keypad
	unsigned char row_pos = 1, col_pos = 0;		// For row and column positon of LCD

	KEY_DDR = 0x0F;		// PINS 0-3 for rows(OUTPUT) and PINs 4-7 for
				// columns(INPUT)
	KEY_PRT = 0xFF;		// Input pull-up resistor		
	lcd_init();
	while(1) {

		if (col_pos == 16 && row_pos == 1) {
			row_pos = 2;
			col_pos = 0;
		}

		if (col_pos == 16 && row_pos == 2) {
			lcd_command(0x01);	// Clear the lcd
			row_pos = 1;
			col_pos = 0;
		}
		
	//	do {
	//		KEY_PRT &= 0xF0;	// Ground all rows at once
	//		asm("NOP");  /*-- requires bc the register store the
	//			      content of previous state ie. of PORT
	//			     (See AVR I/O port in Note) --*/
			/*-- Input ckt of AVR has a delay of 1 clock cycle
			 * ie. PIN register represents the data that was 
			 * present at the pins one clock ago.
			 */
			
	//		colloc = (KEY_PIN & 0xF0);	// Read all comumns
	//	} while(colloc != 0xF0);	// check until all keys released

		do {

			do {
				KEY_PRT &= 0xF0;
				_delay_ms(20);
				colloc = (KEY_PIN & 0xF0);	// See if any key pressed
			} while(colloc == 0xF0);		// Keep checking for key press
			
			_delay_ms(20);		// Call delay for debounce
			colloc = (KEY_PIN & 0xF0);	// Read columns
		} while(colloc == 0xF0);

		while(1) {
			KEY_PRT = 0xFE;		// Ground row 0
			asm("NOP");
			colloc = (KEY_PIN & 0xF0);

			if(colloc != 0xF0) {
				rowloc = 0;	// Save row location
				break;		// Exit while loop
			}


			KEY_PRT = 0xFD;		// Ground row 1
			asm("NOP");
			colloc = (KEY_PIN & 0xF0);

			if(colloc != 0xF0) {
				rowloc = 1;	// Save row location
				break;		// Exit while loop
			}

			KEY_PRT = 0xFB;		// Ground row 2
			asm("NOP");
			colloc = (KEY_PIN & 0xF0);

			if (colloc != 0xF0) {
				rowloc = 2;
				break;
			}

			KEY_PRT = 0xF7;
			asm("NOP");
			colloc = (KEY_PIN & 0xF0);
			if(colloc != 0x0F) {
				rowloc = 3;
				break;
			}
		}

		// Check column and send result to LCD
		if (colloc == 0xE0) {
		        col_pos ++;	
			lcd_gotoxy(col_pos, row_pos);
			lcd_data(keypad[rowloc][0]);
		}

		else if(colloc == 0xD0) {
			col_pos ++;	
			lcd_gotoxy(col_pos, row_pos);
			lcd_data(keypad[rowloc][1]);
		}

		else if(colloc == 0xB0) {
			col_pos ++;	
			lcd_gotoxy(col_pos, row_pos);
			lcd_data(keypad[rowloc][2]);
		}
				
		else if(colloc == 0x70) {
			col_pos ++;	
			lcd_gotoxy(col_pos, row_pos);
			lcd_data(keypad[rowloc][3]);
		}
	}
	return 0;
}

void lcd_command(unsigned char cmnd) {
	LCD_PRT = (LCD_PRT & 0x0F) | (cmnd & 0xF0);  // Kepping the lower nibble
		// as it is and sending higher nibble of the cmnd to the higher
		// nibble of the PORT.
	LCD_PRT &= ~(1 << LCD_RS);	// RS = 0 for command
	LCD_PRT &= ~(1 << LCD_RW);      // RW = 0 for write mode
	LCD_PRT |= (1 << LCD_EN);	// Sending high-to-low pulse in Enable
	_delay_us(1);
	LCD_PRT &= ~(1 << LCD_EN);
	_delay_us(50);

	LCD_PRT = (LCD_PRT & 0x0F) | (cmnd << 4);  // Keeping the lower nibble
		// as it is and sending the lower nibble of the cmd to higher
		// nibble of the PORT.
	LCD_PRT |= (1 << LCD_EN);
	_delay_us(1);				  // For longer pulse
	LCD_PRT &= ~(1 << LCD_EN);
	_delay_us(100);				  // Wait for command to send
}

void lcd_data(unsigned char data) {
	LCD_PRT = (LCD_PRT & 0x0F) | (data & 0xF0);	// Sending higher data
		// nibble.
	LCD_PRT |= (1 << LCD_RS);
	LCD_PRT &= ~(1 << LCD_RW);
	LCD_PRT |= (1 << LCD_EN);
	//_delay_us(1);
	LCD_PRT &= ~(1 << LCD_EN);
	//_delay_us(1);

	LCD_PRT = (LCD_PRT & 0x0F) | (data << 4);	// Sending lower data
		// nibble.
	LCD_PRT |= (1 << LCD_EN);
	_delay_us(1);
	LCD_PRT &= ~(1 << LCD_EN);
	_delay_us(5); 				// Wait for data to send
}

void lcd_init() {
	LCD_DDR = 0xFF;					// LCD port as output
	LCD_PRT &= ~(1 << LCD_EN);
	_delay_us(2000);				// Wait for stable power
	lcd_command(0x33);				// For 4-bit mode
	lcd_command(0x32);				// For 4-bit mode
	lcd_command(0x28);				// For 4-bit mode
	lcd_command(0x0e);				// Display on, cursor on
	lcd_command(0x01);				// Clear LCD
	_delay_us(2000);				// Wait to execute
	lcd_command(0x06);				// Shift cursor right
}

void lcd_gotoxy(unsigned char x, unsigned char y) {	// First column then row
	unsigned char first_char_adr[] = {0x80, 0xC0, 0x94, 0xD4};
		// These are the starting address of each row in LCD as can be
		// seen from datasheet
	lcd_command(first_char_adr[y-1] + x-1);		// To move the cursor to
		// the defined location the address of that location should be
		// send as command for eg. the command for 2nd column 1st row is
		// 0x81.
	_delay_us(100);
}

void lcd_print(char *str) {
	unsigned char i = 0;
	while(str[i] != 0) {
		// 0 is the ACII value for null character
		lcd_data(str[i]);
		i++;
	}
}

void delay_ms(unsigned int delay) {
	//_delay_us(d);
	//*-- This line gives us error because we cannot use the delay function
	//    with the variable value as given in the user manual --* //
	while(delay --) {
    		_delay_ms(1);
	}
}
