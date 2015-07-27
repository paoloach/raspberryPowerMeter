#include "wiringPiSPI.h"

int  wiringPiSPISetup (int channel,  int freq){
	return 1;
}

int wiringPiSPIDataRW(int channel, unsigned char * buffer, int size) {
	return size;
}