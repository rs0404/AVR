# **Using Arduino as FTDI and ISP at once**
**Here I've used Arduino to communicate with atmega328p and computer.
Similarly to make instance changes in the program uploded to avr, I've
used the same Arduino to program the avr. For this I just have to make
some small changes. The detail procedure is shown below.**

### **Step1:**Connect the ICSP headers of the Arduino with the corresponding
avr pins. The description of each pin of ICSP can be seen in the picture.
![ICSP pin descriptions](https://qph.fs.quoracdn.net/main-qimg-0b9e61e4598a113b72673f73116b0ad7.webp)

Note: All pins of ICSP are to be connected to the corresponding pins of avr
except the reset pin. Instead connect pin 10 of arduino to reset pin of avr.
Similarly connecting  AVCC and GND pin with the power pins is recommended.
Moreover connecting 100pF capacitors across both the power pins also 
recommended for stable voltage supply across avr.

![Connecting Capacitor in VCC and GND pin](https://lh3.googleusercontent.com/proxy/BDKie5-zPX1b1MY-b-QoeNyWh4xJY1DvsyWrR7OukRCzMDIjTikp6d7ZX7wocK0r3rB_4XMI3oNmG8InMWSNOvD7fdXWnEEcSAO7pV8k6dZKXEK6ps5YPzlR_bmV2c8-ze8za-iKIw)

This picture is also not complete enought as AVCC and GND not connected with 
power source and 100pF capacitor.

### **Step2:**Connect the 16MHz crystals and 22pF capacitors as shown in diagram.
![Crystal and capacitor connection with avr](https://4.bp.blogspot.com/-0q_16rv2Wj0/ToJ0pvJbwlI/AAAAAAAAAJk/IE9yuSTi4Yk/s1600/untitled1.PNG)
![Real connection](https://arduinoelectronics.files.wordpress.com/2012/02/arduinoisp1.png)

This is necessary because arduion works on 16 MHz crystal and while doing serial
communication with avr and computer arduino functions as curier boy so to
synchronize with arduino it is necessary otherwise random characters will be 
desplayed.

### **Step3:**Uplode the code "ArduinoISP" found inside the examples in
Arduino.

### **Step4:**Install Mighty core for atmega328p, select the correct chip,
Programmer: Arduino as ISP and then burn bootloader.
Burning bootloader is necessary to configure avr to work with 16 MHz crystal
or to make suitable to use with Arduino.

### **Step5:**Uplode the code (code is found above in the file name "test.ino".
Don't forget to press Shift otherwise it will uplode to Arduino instead of avr.

### **Step6:**Connect RESET pin of arduino to ground and tx pin of avr to tx pin
of Arduino. Open the serial monitor and observe the output and check if it is as
expected or not.
For this code output should be "roshan SHRESTHA ", and baud rate is 9600 as 
specified in the code.

### **Step7:**To change the output form "roshan SHRESTHA" to "Roshan Shrestha",
first disconnect the RESET pin of Arduino which is connected to ground and TX
pin as well.
	Make change to the line UART_transmit_string("roshan SHRESTHA ") as 
desired like: UART_transmit_string("Roshan Shrestha ").
	Uplode to AVR by pressing Shift.
	Reconnect the RESET pin with GND and the TX pin.
	Observe the output in the serial monitior of Arduino IDE. Output should
be as expected.

Final circuit:


