#include <stdio.h>
#include <wiringPiSPI.h>
#include <ncurses.h>

typedef unsigned char byte;

union _Register {
	unsigned int value;
	byte bytes[4];
};

typedef union _Register Register;


unsigned int readRegister(unsigned char reg);
Register getRegister(byte reg);
void readAllRegister(void);
void readPage1(void);
void writePage1(int reg, int value);	
char getcmd(void);
void printHelp(void);
void setPage0(void);
void printIstantaneusCurrent(void);
void printIstantaneusVolt(void);
void printIstantaneusPower(void);
void printRealPower(void);
void printRMSVolt(void);
void printRMSCurrent(void);
double binConvert(Register * reg, double pow2);
double range_1_sign(Register * currentRegister);

int main() {
	int initSPI =  wiringPiSPISetup (0, 500000) ;
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, TRUE);

	printHelp();

	int i=0;
	while(1){
		int ch = getch();
		if (ch 	!= ERR){
			printw("press %d",ch);
			switch(ch){
				case 'q':
					endwin();
					return 0;
			}
		}
		refresh();
		setPage0();
		printIstantaneusCurrent();
		printIstantaneusVolt();
		printIstantaneusPower();
		printRealPower();
		printRMSVolt();
		printRMSCurrent();
		i++;
	}
	endwin();
	return 0;
}

void printHelp(){
	move(0,0);
	printw("q for exit\n");
	printw("0 read all registers page 0\n");
	printw("1 read register page 1\n");
	printw("s performe single computation cycle\n");
	printw("c performe continuo compuation cycle\n");
	printw("t set Temperature sensor gain or offset\n");
}

void setPage0(){
	unsigned char buffer[4] = {0x7E,0x00,0x00,0x0};
    wiringPiSPIDataRW(0,buffer,4);
}

void printIstantaneusCurrent(){
	Register currentRegister = getRegister(7);
	move(0,50);
	printw("    current %0.9f", range_1_sign(&currentRegister));
	
}

void printIstantaneusVolt(){
	Register currentRegister = getRegister(8);
	move(0,75);
	printw("    volt %0.9f", range_1_sign(&currentRegister));
}

void printIstantaneusPower(){
	Register currentRegister = getRegister(9);
	move(0,100);
	printw("ist. power %0.9f", range_1_sign(&currentRegister));
}

void printRealPower(void) {
	Register currentRegister = getRegister(10);
	move(1,100);
	printw("real power %0.9f", range_1_sign(&currentRegister));
}

void printRMSVolt(void){
	Register currentRegister = getRegister(11);
	move(1,50);
	printw("rms current %0.9f", binConvert(&currentRegister, 0.5));
}
void printRMSCurrent(void){
	Register currentRegister = getRegister(12);
	move(1,75);
	printw("rms volt %0.9f", binConvert(&currentRegister, 0.5));
}

double range_1_sign(Register * currentRegister){
	int sign = currentRegister->bytes[1] & 0x80;
	currentRegister->bytes[1] = currentRegister->bytes[1] & 0x7F;
	double current = binConvert(currentRegister, 1);
	if (currentRegister->bytes[1] & 0x80){
		current = -current;
	}
	return current;
}

double binConvert(Register * reg, double pow2) {
	unsigned char mask = 0x80;
	double res=0;

	do {
		if ((reg->bytes[1] & mask) != 0){
			res += pow2;
		}
		pow2 = pow2 / 2;
		mask = mask >> 1;
	} while (mask != 0);

	mask = 0x80;
	do {
		if ((reg->bytes[2] & mask) != 0){
			res += pow2;
		}
		pow2 = pow2 / 2;
		mask = mask >> 1;
	} while (mask != 0);
	mask = 0x80;
	do {
		if ((reg->bytes[3] & mask) != 0){
			res += pow2;
		}
		pow2 = pow2 / 2;
		mask = mask >> 1;
	} while (mask != 0);
	return res;
}

int  spi(){
    int result;

    int initSPI =  wiringPiSPISetup (0, 500000) ;
    printf("init is %d\n", initSPI);
   unsigned char buffer[4];

	
   printf("q for exit\n");
   printf("0 read all registers page 0\n");
   printf("1 read register page 1\n");
   printf("s performe single computation cycle\n");
   printf("c performe continuo compuation cycle\n");
   printf("t set Temperature sensor gain or offset\n");
 


   while(1){
	 
	char cmd = getcmd();
	int reg;
	int value;

	switch(cmd) {
	    case 'q':
		return 0;
	   case '0':
		readAllRegister();
		break;
	   case 's':
		buffer[0] = 0xE0;
		wiringPiSPIDataRW(0,buffer, 1);
		break;
	   case 'c':
		buffer[0] = 0xE8;
		wiringPiSPIDataRW(0,buffer,1);
		break;
	   case '1':
		readPage1();
		break;
	   case 't':
		   printf("\ng gain, o offset");
		   cmd = getcmd();
		   
		   if (cmd == 'g'){
			   reg = 2;
		   } else if (cmd == 'o'){
			   reg = 3;
		   } else {
			   printf("invalid chose\n");
			   break;
		   }
		   
		   scanf("%d",&value);
		   writePage1(reg,value);
		   break;
			
	}
   }


}

void writePage1(int reg, int value){
	unsigned char bufferPage[4] = {0x7E,0x00,0x00,0x1};
	const unsigned char msb = (value & 0xFF0000) >> 16;
	const unsigned char mediumByte = (value & 0xFF00) >> 8;
	const unsigned char lowByte = value & 0xFF;
	unsigned char bufferReg[4] = {0x40 | (reg << 1),msb, mediumByte, lowByte};
	printf("%02X %02X %02X %02X\n", bufferReg[0], bufferReg[1], bufferReg[2], bufferReg[3]);
	wiringPiSPIDataRW(0,bufferPage,4);
	wiringPiSPIDataRW(0,bufferReg,4);
}

void readPage1(void){
    unsigned char buffer[4] = {0x7E,0x00,0x00,0x1};
    wiringPiSPIDataRW(0,buffer,4);
    printf("Energy Pulse Output Width: -->");
    readRegister(0);
    printf("No Load Threshold: --> ");
    readRegister(1);
    printf("Temperature sensor gain: --> ");
    readRegister(2);
    printf("Temperature Sensor offset: --> ");
    readRegister(3);
        
}

void readAllRegister(void) {
	int i;
    unsigned char buffer[4] = {0x7E,0x00,0x00,0x0};
    wiringPiSPIDataRW(0,buffer,4);	

    for (i=0; i <= 31; i++){
        printf("read register %d) --> ", i);
        readRegister(i);
    }
}

unsigned int  readRegister(unsigned char reg){
    unsigned char buffer[4] = {reg<<1, 0xFF, 0xFF,0xFF};

    wiringPiSPIDataRW(0,buffer, 4);
    printf("%02X%02X%02X\n", buffer[1], buffer[2], buffer[3]);
    return (((unsigned int)buffer[1]) << 16) + (((unsigned int)buffer[2]) << 8) + buffer[3];

}

Register getRegister(byte reg) {
	unsigned char buffer[4] = {reg<<1, 0xFF, 0xFF,0xFF};
	Register result;
	
    wiringPiSPIDataRW(0,buffer, 4);
	
	result.bytes[0] = 0;
	result.bytes[1] = buffer[1];
	result.bytes[2] = buffer[2];
	result.bytes[3] = buffer[3];
    return result;
}

char getcmd(void) {
	char cmd =0;
	do {
		cmd = getchar();
	} while (cmd < 32);
	return cmd;
}