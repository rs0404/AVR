# **Arduino as ISP**

## **ISP**
While in earlier days a chip had been removed from the target board and 
placed in a programming device, today's microcontrollers and external 
memory chips can be re-programmed without being removed from the 
circuit. This is called in-ciruit or in-system programming.
In-System Programming (ISP), also called in-circuit serial programming
(ICSP), is the ability of some programmable logic devices, microcontrollers,
and other embedded devices to be programmed while installed in a complete
system (no need to remove the chip and kept in separate programming circuit).

## **Programming AVR using Arduino**
Well if we want to work with AVR instead of Arduino for our projects, then
what we need is AVR, AVR programmer and other components as required for
your project. I say, if you already have an Arduino with you then you 
don't need another AVR programmer like USBasp, FTDI etc. You can use your
Arduino to program your AVR. You can follow my steps to do so:


/*--Steps to follow --*/
************************
 First Part (Make your arduino to act as programmer)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
1. Connect your arduino to your computer.
2. Open Arduino IDE.
3. Click on File go to Examples.
4. Goto ArduinoISP (may be at no 11 of examples and click on ArduinoISP.
5. A new arduino IDE will appear with ArduinoISP codes.
6. Uplode the code to your arduino.

----------------------------------------------------------------------------------------------------

 Second Part (Circuit Connections)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
(Note: Circuit connection is as refered to atmega32a but can be done with other AVRs as well with the
help of pin diagram of that microcontroller, which tells which pin is what.
Knowing the pin no of MOSI, MISO, SCK, RESET, VCC and GND we can have connection to any AVR)
1. Connect digital pin 13 of your arduino ie. SCK pin to pin 8 or (PB7) or SCK pin of AtMega32a.
   (In case of atmega328p pin 19 or PB5).
2. Arduino pin 12 (MISO) to atmega32a pin 7 (PB6, MISO) or atmega328p pin 18 (PB4, MISO).
3. Arduino pin 11 (MOSI) to atmega32a pin 6 (PB5, MOSI) or atmega328p pin 17 (PB3, MOSI).
4. Arduino pin 10 (SS)   to atmega32a pin 9 (RESET')    or atmega328p pin 1  (RESET').
5. Arduino 5V pin        to atmega32a pin 10 (VCC)      or atmega328p pin 7  (VCC).
6. Arduino GND pin 	 to atmega32a pin 11 (GND)      or atmega328p pin 8  (GND).

-----------------------------------------------------------------------------------------------------

Third part (Make Arduino IDE ready to accept the program written for your avr (for Atmega32a)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
1. Click on File.
2. Click on Preferences.
3. Preferences dialog box will appear.
4. In "Additional Boards Manager URLs" box copy the link:
	 https://mcudude.github.io/MightyCore/package_MCUdude_MightyCore_index.json (for Atmega32a)
	 https://mcudude.github.io/MiniCore/package_MCUdude_MiniCore_index.json (for Atmega32a)
5. After that click OK.
6. Click on Tools and go to Board
7. Click on Boards Manager. Then wait for platform indexes to finish downloding.
8. Search for MightyCore in that list by typing on the search bar or scrolling (may be at the last so 
   scroll to last at first).
9. Click on MightyCore and then click on install.
10. Wait till it is installed.
11. Close the Preferences dialog box.

/*--Manual Installation

Go to the link: https://github.com/MCUdude/MiniCore
Click on the "Download ZIP" button in the upper right corner. Exctract the ZIP file, and move the 
extracted folder to the location "~/Documents/Arduino/hardware" (for Windows, for 
Linux "~/sketchbook/hardware"). Create the "hardware" folder if it doesn't exist. 
Open Arduino IDE, and a new category in the boards menu called "MiniCore" will 
show up. --*/

12. Click on Tools again and go to Board.
13. Now you will see ATmega32 and some other names of AVR added in list.
14. Click on ATmega32.
15. Again click on Toos and go to Programmer.
16. Click "Arduino as ISP" as then you will see Programmer:"Arduino as ISP".
17. Write the program to uplode into the AVR. 
18. Press shift and then click on Upload.
	Tada
	

/*-- To show option of both atmega32 and atmega328 at once --*/
-> First process as above for atmega32.
-> Now go to the site https://www.arduino.cc/en/Tutorial/ArduinoToBreadboard.
-> Scrol down and reach to the topic "Minimal Circuit".
-> In 1. of that topic you will see "breadboard-1-6-x.zip" and some other written
   in blue. Download "breadboard-1-6-x.zip".
-> Extract that to "~/sketchbook/hardware" in linux or see the location in the 
   Arduino preferences dialog.
-> Restart the arduino and will see both atmega32 and atmega328 in Board.
	Ta-da your work is done.
