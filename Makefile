raspberryPowerMeter: main.o
	gcc -O2  main.o -o raspberryPowerMeter -L. -lwiringPi -lncurses

main.o: main.c
	gcc -I. -O2 main.c -c -o main.o

wiringMock:
	gcc -shared -I. -O2  -fpic wiringPiSPIMock.c  -Wl,-soname,libwiringPi.so -o libwiringPi.so

lib: wiringMock


clean:
	rm -f *.o
	rm -f raspberryPowerMeter
	rm -f *.so