#include "characterLCD.h"
#define F_CPU 1000000UL // This line should be above the delay.h
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
#include <inttypes.h>

// When the display powers up, it is configued as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 8-bit interface data
//    N = 0; 1-line display
//    F = 0; 5*8 dot character font
// 3. Display on/off control;
//    D = 0; Display off
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
//
// Note, however, that resetting the microcontroller doesn't reset the LCD, so
// we can't assume that its in that state when a sketch starts (and the
// characterLCD constructor is called).

characterLCD::characterLCD(volatile uint8_t *cmdPort, volatile uint8_t *dataPort,
			   uint8_t rs, uint8_t enable,
		           uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
			   uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
  init(cmdPort, dataPort, 0, rs, 255, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

characterLCD::characterLCD(volatile uint8_t *cmdPort, volatile uint8_t *dataPort,
			   uint8_t rs, uint8_t rw, uint8_t enable,
			   uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
  init(cmdPort, dataPort, 1, rs, rw, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

characterLCD::characterLCD(volatile uint8_t *cmdPort, volatile uint8_t *dataPort,
			   uint8_t rs, uint8_t rw, uint8_t enable,
		           uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
			   uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
  init(cmdPort, dataPort, 0, rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

characterLCD::characterLCD(volatile uint8_t *cmdPort, volatile uint8_t *dataPort,
			   uint8_t rs,  uint8_t enable,
			   uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
  init(cmdPort, dataPort, 1, rs, 255, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

void characterLCD::init(volatile uint8_t *cmdPort, volatile uint8_t *dataPort,
			uint8_t fourBitMode, uint8_t rs, uint8_t rw, 
			uint8_t enable,
		        uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
			uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
  _cmd_prt = cmdPort;
  _data_prt = dataPort;
  _cmd_ddr = (_cmd_prt - 1);
  _cmd_pin = (_cmd_prt - 2);
  _data_ddr = (_data_prt - 1);
  _data_pin = (_data_prt - 2);

  _rs_pin = rs;
  _rw_pin = rw;
  _enable_pin = enable;
  
  _data_pins[0] = d0;
  _data_pins[1] = d1;
  _data_pins[2] = d2;
  _data_pins[3] = d3;
  _data_pins[4] = d4;
  _data_pins[5] = d5;
  _data_pins[6] = d6;
  _data_pins[7] = d7;

  if (fourBitMode)
    _displayFunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

  else
    _displayFunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;

  begin(16, 1);
}

void characterLCD::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
  if (lines > 1) {
    _displayFunction |= LCD_2LINE;
  }
  _numLines = lines;

  setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);	// Starting
    // DDRAM address of each row.

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != LCD_5x8DOTS) && (lines == 1)) {
    _displayFunction |= LCD_5x10DOTS;
  }

  *_cmd_ddr |= (1 << _rs_pin);	// Set RS pin to output.
  if (_rw_pin != 255) {		// 255 is a random value.
    *_cmd_ddr |= (1 << _rw_pin);
  }
  *_cmd_ddr |= (1 << _enable_pin);

  // Do these once. instead of every time a characte id drawn for speed reasons.-
  for (unsigned char i=0; i < ((_displayFunction & LCD_8BITMODE) ? 8 : 4); ++i)
  {
    *_data_ddr |= (1 << _data_pins[i]);
  }

  // See page 45/46 for Initialization Specification
  // According to datasheet, we need at least 40 ms after the power rises above
  // 2.7V before sending commands.
  _delay_ms(50);
  // Now we pull both RS and R/W low to begin commands
  *_cmd_prt &= ~(1 << _rs_pin);
  *_cmd_prt &= ~(1 << _enable_pin);
  if (_rw_pin != 255) {
    *_cmd_prt &= ~(1 << _rw_pin);
  }

  // Put the LCD into 4 bit or 8 bit mode
  if(! (_displayFunction & LCD_8BITMODE)) {	// 4 bit mode
    // This is according to the Hitachi HD44780 data sheet fig 24, pg 46
    // We start in 8bit mode, try to set 4 bit mode
    write4bits(0x03);
    _delay_us(4500);	// Wait min 4.1ms

    // Second try
    write4bits(0x03);
    _delay_us(4500);

    // Third try
    write4bits(0x03);
    _delay_us(150);

    // Finally, set to 4-bit interface
    write4bits(0x02);
  } 

  else {
    // This is according to the Hitachi HD44780 data sheet pg 45 fig 23
    //  Send fucntion set command sequence
    command(LCD_FUNCTIONSET | _displayFunction);
    _delay_us(4500);  // wait more than 4.1ms

    // second try
    command(LCD_FUNCTIONSET | _displayFunction);
    _delay_us(150);

    // third go
    command(LCD_FUNCTIONSET | _displayFunction);
  }

  // Finally, set # lines, font size, etc.
  command(LCD_FUNCTIONSET | _displayFunction);

  // Turn the display on with cursor or blinking default
  _displayControl = LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKOFF;
  display();

  // Clear it off
  clear();

  // Initialize to default text direction
  _displayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // Set the entry mode
  command(LCD_ENTRYMODESET | _displayMode);
}


void characterLCD::setRowOffsets(int row0, int row1, int row2, int row3)
{
  _row_offsets[0] = row0;
  _row_offsets[1] = row1;
  _row_offsets[2] = row2;
  _row_offsets[3] = row3;
}

/*-- High Level Commands, for the user! --*/
void characterLCD::clear()
{
  command(LCD_CLEARDISPLAY);	// Clear display, set cursor position to zero
  _delay_us(2000);
}

void characterLCD::home()
{
  command(LCD_RETURNHOME);	// Set cursor position to zero
  _delay_us(2000);
}

void characterLCD::setCursor(uint8_t col, uint8_t row)
{
  const size_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
  if ( row >= max_lines ) {
    row = max_lines - 1;    // we count rows starting w/0
  }
  if ( row >= _numLines ) {
    row = _numLines - 1;    // we count rows starting w/0
  }
  
  command(LCD_SETDDRAMADDR | (col + _row_offsets[row]));
}

// Turn the display on/off (quickly)
void characterLCD::noDisplay() {
  _displayControl &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displayControl);
}
void characterLCD::display() {
  _displayControl |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displayControl);
}

// Turns the underline cursor on/off
void characterLCD::noCursor() {
  _displayControl &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displayControl);
}
void characterLCD::cursor() {
  _displayControl |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displayControl);
}

// Turn on and off the blinking cursor
void characterLCD::noBlink() {
  _displayControl &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displayControl);
}
void characterLCD::blink() {
  _displayControl |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displayControl);
}

// These commands scroll the display without changing the RAM
void characterLCD::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void characterLCD::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void characterLCD::leftToRight(void) {
  _displayMode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displayMode);
}

// This is for text that flows Right to Left
void characterLCD::rightToLeft(void) {
  _displayMode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displayMode);
}

// This will 'right justify' text from the cursor
void characterLCD::autoScroll(void) {
  _displayMode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displayMode);
}

// This will 'left justify' text from the cursor
void characterLCD::noAutoScroll(void) {
  _displayMode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displayMode);
}

/*********** mid level commands, for sending data/cmds */

inline void characterLCD::command(uint8_t value) {
  send(value, 0);
}

/*inline size_t characterLCD::write(uint8_t value) {
  send(value, 1);
  return 1; // assume sucess
}*/

/************ low level data pushing commands **********/
void characterLCD::print(const char *str)
{
  register char c;	// With register keyword the mcu is able  to excess
	// the variable faster and it can't be declared as global variable.
  while((c = *str++)) {
    send(c, 1);
  } 
}

void characterLCD::print(int num)
{
  char buffer[6];	// Store 5 digits integer value and remaining 1
  	// bit to store null character.
  itoa(num, buffer, 10);
  print(buffer);
}

// write either command or data, with automatic 4/8-bit selection
void characterLCD::send(uint8_t value, uint8_t mode) {
  if (mode) {
    *_cmd_prt |= (1 << _rs_pin);
  }

  else {
    *_cmd_prt &= ~(1 << _rs_pin);
  }
  // if there is a RW pin indicated, set it low to Write
  if (_rw_pin != 255) { 
    *_cmd_prt &= ~(1 << _rw_pin);
  }
  
  if (_displayFunction & LCD_8BITMODE) {
    write8bits(value); 
  }
 
  else {
    write4bits(value >> 4);
    write4bits(value);
  }
}

void characterLCD::pulseEnable(void) {
  *_cmd_prt &= ~(1 << _enable_pin);
  _delay_us(1);    
  *_cmd_prt |= (1 << _enable_pin);
  _delay_us(1);    // enable pulse must be >450ns
  *_cmd_prt &= ~(1 << _enable_pin);
  _delay_us(100);   // commands need > 37us to settle
}

void characterLCD::write4bits(uint8_t value) {
  for (int i = 0; i < 4; i++) {
    if ((value >> i) & 0x01)
      *_data_prt |= (1 << _data_pins[i]);
    else
      *_data_prt &= ~(1 << _data_pins[i]);
  }
    
  pulseEnable();
}

void characterLCD::write8bits(uint8_t value) {
  for (int i = 0; i < 8; i++) {
    if((value >> i) & 0x01){ 
      *_data_prt |= (1 << _data_pins[i]);
       }
    else
      *_data_prt &= ~(1 << _data_pins[i]);
  }
  
  pulseEnable();
}
