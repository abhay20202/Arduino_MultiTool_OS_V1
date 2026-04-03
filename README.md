## This Code Is Compatible for Arduino nano as this runs in 32kb Storage and 2 kb ram.

## Dependencies--
This project requires the following Arduino libraries:
* Adafruit SSD1306
* Adafruit GFX

## Components --
* 0.96" SSD1306 OLED (128x64) I2C Pins 
* HC-SR04 Ultrasonic Sensor
* Tactile Push Buttons
* EMF Probe (Wire/Antenna) to Analog Pin A0 

## Wiring`s -- 


*For HC-SR04 Ultrasonic Sensor -

VCC   ---   5V,

Trig   ---   D9,

Echo   ---   D10,

GND   ---   GND,


* For Screen -

VCC   ---   3.3V or 5V (if screen Supports)

GND   ---   GND

SCL   ---   A5

SDA   ---   A4


* For Buttons -

Up        ---   D2,

Down   ---   D3,

Left      ---   D4,

Right    ---   D5,

Select  ---   D6,

Back    ---   D7,


And Every Other pin for Button should be connected to Arduino`s GND pin.


* EMF Probe -
 
One_End   ---   A0,

Other end should set free, as it will detect and tell A0 pin for Static... Which will be shown on the Screen.
