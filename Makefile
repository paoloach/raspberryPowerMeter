raspberryPowerMeter: main.o
	gcc -O2 main.o -o raspberryPowerMeter -L. -lwiringPi -lncurses

main.o: main.c
	gcc -I. -O2 main.c -c -o main.o

wiringMock:
	gcc -shared -I. -fpic wiringPiSPIMock.c  -Wl,-soname,libwiringPi.so -o libwiringPi.so