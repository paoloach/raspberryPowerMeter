# raspberryPowerMeter
Simple program to interface to CS5463, a power meter chips, in order to play with its varius funtionality
The aim is to allow the programmer to play with this chip so it a little rought,also the ncurses library allowing a simple interface also into a console

The CS5463 is a power meter chip with a SPI interface. It measure the istantaneus current and voltage and calculate the power (as I*V).
It also measure the mean over the seconds of these values.
It has some optional filters in order to filter out the bias.

The SPI interface allowing to read and write the registers the affect the behaviour. 

The interface with raspberry is very simple. It is enough to use 4 wire: ground, sck, scin and scout that are present on GPIO connector of raspberry and on the relative pin of CS5463
To side CS5463, is enough a 5 volt power to connect to the right pins, a quartz (fram 1 to 5 Mhz) without capacitors (they are include into the die) et voilà, the raspberry and CS5463 are communicating.

Obvioulsy the CS5463 doesn't measure any because its pins are floating.



The program uses the SPI interface, channel 0, to comunicate with the chip
It use the SPI module of wiringPi library, so to work correctly, it is need to load the SPI module
In order to allowing a source developing into  a x86 PC, there is also a wiringPi mock library to be created using

make wiringMock



