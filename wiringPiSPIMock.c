#include "wiringPiSPI.h"

int pageRegister=0;

unsigned int reg0[32] = {
	0x000001, 0x000000, 0x400000,0x000000,0x400000,0x000FA0,0x800000,0x000000,
	0x000000, 0x000000, 0x000000,0x000000,0x000000,0x01999A,0x000000,0x800001,
	0x000000, 0x000000, 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000,
	0x000000, 0x000000, 0x000000,0x000000,0x000000,0x000000,0x000000,0x000000
};

int  wiringPiSPISetup (int channel,  int freq){
	return 1;
}

int wiringPiSPIDataRW(int channel, unsigned char * buffer, int size) {
	if (size == 4){
		int reg = (buffer[0] >> 1) & 0x1F;
		if (!(buffer[0] & 0x40)){
			switch(pageRegister){
				case 0:
					buffer[1] = (reg0[reg] & 0xFF0000) >> 16;
					buffer[2] = (reg0[reg] & 0xFF00) >> 8;
					buffer[3] = (reg0[reg] & 0xFF);
					return 4;
				case 1:
				case 2:
					buffer[1] = 0;
					buffer[2] = 0;
					buffer[3] = 0;
					return 4;
			}
		}
	}
	return size;
}