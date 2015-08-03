#include <stdio.h>
#include <wiringPiSPI.h>
#include <ncurses.h>

typedef unsigned char byte;

union _Register {
	unsigned int value;
	byte bytes[4];
};

typedef union _Register Register;

#define text_size 20
#define val_size  12;

int col1 = 50;
int col1_v;
int col2;
int col2_v;
int col3;;
int col3_v;
int col4;
int col4_v;
int col5;
int col5_v;

int bit_row = 8;

float voltFactor=1;
float CurrFactor=1;


unsigned int readRegister(unsigned char reg);
Register getRegister(byte reg);
void readAllRegister(void);
void readPage1(void);
void writePage1(int reg, int value);	
char getcmd(void);
void printHelp(void);
void setPage0(void);
void printConfiguration(void);
void printCurrent(void);
void printVolt(void);
void printCurrentOffset(void);
void printVoltOffset(void);
void printCycleCount(void);
void printCurrentGain(void);
void printVoltGain(void);
void pulseRage(void);
void printCycleCount(void);
void printEpsilon(void);
void printPowerOffset(void);
void printStatus(void);
void printMask(void);
void printCurrentACOffset(void);
void printVoltageACOffset(void);
void printOperationMode(void);
void printTemperature(void);
void printPower();
void printFloat(int y, int col, const char * text, double val);

double binConvert(Register * reg, double pow2);
double range_1_sign(Register * currentRegister);

int main() {
	
	col1_v = col1 + text_size;
	col2 = col1_v + val_size;
	col2_v = col2+text_size;
	col3 = col2_v + val_size;
	col3_v = col3+text_size;
	col4 = col3_v + val_size;
	col4_v = col4+text_size;
	col5 = col4_v + val_size;
	col5_v = col5 + text_size;
	unsigned char buffer[4];

	
	
	int initSPI =  wiringPiSPISetup (0, 500000) ;
	initscr();
	start_color();
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
				case 's':
					buffer[0] = 0xE0;
					wiringPiSPIDataRW(0,buffer, 1);
					break;
				case 'c':
					buffer[0] = 0xE8;
					wiringPiSPIDataRW(0,buffer,1);
					break;
				case '1':{
						Register currentRegister = getRegister(0);
						currentRegister.bytes[0] |= 0x40;
						currentRegister.bytes[1] &= 0xFE;
						wiringPiSPIDataRW(0,currentRegister.bytes,4);
						break;
					}
				case '5':{
						Register currentRegister = getRegister(0);
						currentRegister.bytes[0] |= 0x40;
						currentRegister.bytes[1] |= 0x01;
						wiringPiSPIDataRW(0,currentRegister.bytes,4);
						break;
					}
				case 'p':
					buffer[0] = 0xC0;
					wiringPiSPIDataRW(0,buffer,1);
					break;
				case 'r':  // reset
					buffer[0] =0x80;
					wiringPiSPIDataRW(0,buffer,1);
					break;
				case 'x':  // stand-by
					buffer[0] =0x88;
					wiringPiSPIDataRW(0,buffer,1);
					break;
				case 'z': // sleep
					buffer[0] =0x90;
					wiringPiSPIDataRW(0,buffer,1);
					break;
				}
		}
		refresh();
		setPage0();
		printConfiguration();
		printCurrent();
		printVolt();
		printCurrentOffset();
		printVoltOffset();
		printCurrentGain();
		printVoltGain();
		printCycleCount();
		printEpsilon();
		pulseRage();
		printStatus();
		printMask();
		printPower();
		printCurrentACOffset();
		printVoltageACOffset();
		printOperationMode();
		printTemperature();
		i++;
	}
	endwin();
	return 0;
}

void printHelp(){
	move(0,0);
	printw("q for exit\n");
	printw("s performe single computation cycle\n");
	printw("c performe continuo compuation cycle\n");
	printw("p power-up/HALT");
	printw("r reset\n");
	printw("x stand-by\n");
	printw("z sleep\n");
	printw("t set Temperature sensor gain or offset\n");
	printw("1 set current gain to x10t\n");
	printw("5 set current gain to x50t\n");
}

void setPage0(){
	unsigned char buffer[4] = {0x7E,0x00,0x00,0x0};
    wiringPiSPIDataRW(0,buffer,4);
}



void printIstantaneusCurrent(){
	
}

void printVolt(){
	Register voltReg = getRegister(8);
	printFloat(0, col1, "Volt", range_1_sign(&voltReg));
	voltReg = getRegister(12);
	printFloat(0, col2, "RMS Volt", binConvert(&voltReg, 0.5));
	voltReg = getRegister(23);
	printFloat(0, col3, "Peak Volt", range_1_sign(&voltReg));
}


void printCurrent(void){
	Register currentRegister = getRegister(7);
	printFloat(1, col1, "Current", range_1_sign(&currentRegister));
	currentRegister = getRegister(11);
	printFloat(1, col2, "RMS Current", binConvert(&currentRegister, 0.5));
	currentRegister = getRegister(22);
	printFloat(1, col3, "Peak Current", range_1_sign(&currentRegister));

}


// volt calibraion
void printVoltGain(void) {
	Register voltRegister = getRegister(4);
	printFloat(5, col1, "Volt. Gain", binConvert(&voltRegister,2));
}

void printVoltOffset(void) {
	Register voltRegister = getRegister(3);
	printFloat(5, col2, "Volt. Offset", range_1_sign(&voltRegister));
}

void printVoltageACOffset(void) {
	Register voltRegister = getRegister(17);
	printFloat(5, col3, "Volt. AC Offset", range_1_sign(&voltRegister));
}

// current calibration
void printCurrentGain(void) {
	Register currentRegister = getRegister(2);
	printFloat(6, col1, "Curr. Gain", binConvert(&currentRegister,2));
}

void printCurrentOffset(void) {
	Register currentRegister = getRegister(1);
	printFloat(6, col2, "Curr. Offset", range_1_sign(&currentRegister));
}

void printCurrentACOffset(void) {
	Register powerRegister = getRegister(16);
	printFloat(6, col3, "Curr. AC Offset", range_1_sign(&powerRegister));
}



void printPower(){
	Register powerReg = getRegister(9);
	printFloat(2, col1, "Inst Power", range_1_sign(&powerReg));
	powerReg = getRegister(14);
	printFloat(4, col5, "Power Offset", range_1_sign(&powerReg));
	powerReg = getRegister(10);
	printFloat(3, col1, "Real Power", range_1_sign(&powerReg));
	powerReg = getRegister(21);
	printFloat(2, col2, "Inst React Power", range_1_sign(&powerReg));
	powerReg = getRegister(20);
	printFloat(3, col2, "Mean React Power", range_1_sign(&powerReg));
	powerReg = getRegister(24);
	printFloat(3, col3, "Reactive Power(Q)", range_1_sign(&powerReg));
	powerReg = getRegister(25);
	printFloat(3, col4, "Power Factor(PF)", range_1_sign(&powerReg));
	powerReg = getRegister(27);
	printFloat(3, col5, "Apparent Power(S)", range_1_sign(&powerReg));
	powerReg = getRegister(29);
	printFloat(2, col3, "Harm. Power(Ph)", range_1_sign(&powerReg));
	powerReg = getRegister(30);
	printFloat(2, col4, "Fund Active Power(Pf)", range_1_sign(&powerReg));
	powerReg = getRegister(31);
	printFloat(2, col5, "Fund Reactive Power(Qh)", range_1_sign(&powerReg));
}

void printCycleCount(void) {
	Register countRegister = getRegister(5);
	unsigned int val = (((unsigned int)countRegister.bytes[1]) * 0x10000)+(((unsigned int)countRegister.bytes[2]) * 0x100)+(((unsigned int)countRegister.bytes[3]));
	mvprintw(4, col1, "Cycle Count %d", val);
}

void pulseRage(void) {
	Register pulseRegister = getRegister(6);
	mvprintw(4, col2, "pulse RateE");
	mvprintw(4, col2_v, "%0.9f", range_1_sign(&pulseRegister));
}

void printEpsilon(void){
	Register pulseRegister = getRegister(13);
	mvprintw(4, col3, "epsilon");
	mvprintw(4, col3_v, "%0.9f", range_1_sign(&pulseRegister));
}

void printTemperature(void) {
	Register currentRegister = getRegister(19);
	int sign = (currentRegister.bytes[3] & 0x80) ? -1 : 1;
	currentRegister.bytes[3] = currentRegister.bytes[3] & 0x7F;
	double val = binConvert(&currentRegister,128) * sign;;
	mvprintw(4, col4, "Temperature");
	mvprintw(4, col4_v, "%0.9f", val);
}



void printFloat(int y, int col, const char * text, double val){
	mvprintw(y, col, text);
	mvprintw(y, col+text_size, "%0.9f", val);
}


void printOperationMode(void){
	int row=bit_row+1;
	Register status = getRegister(18);
	if (status.bytes[2] & 0x02){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1, "E2MODE");
	
	if (status.bytes[2] & 0x01){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+7, "XVDEL");
	
	if (status.bytes[3] & 0x80){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+13, "XIDEL");
	
	if (status.bytes[3] & 0x40){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+19, "IHPF");

	if (status.bytes[3] & 0x20){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+24, "VHPF");
	
	if (status.bytes[3] & 0x10){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+29, "IIR");
	
	if (status.bytes[3] & 0x08){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+33, "E3MODE1");
	
	if (status.bytes[3] & 0x04){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+41, "E3MODE0");
	
	if (status.bytes[3] & 0x02){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+49, "POS");
	
	if (status.bytes[3] & 0x01){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+53, "AFC");
}

void printStatus(void) {
	int row=bit_row;
	Register status = getRegister(15);
	if (status.bytes[1] & 0x80){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1, "DRDY");
	attrset(A_NORMAL);
	
	if (status.bytes[1] & 0x08){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+5, "CRDY");
	attrset(A_NORMAL);
	
	if (status.bytes[1] & 0x02){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+9, "IOR");
	attrset(A_NORMAL);
	
	if (status.bytes[1] & 0x01){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+13, "VOR");
	attrset(A_NORMAL);
	
	
	if (status.bytes[2] & 0x40){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+17, "IROR");
	attrset(A_NORMAL);
	
	if (status.bytes[2] & 0x20){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+22, "VROR");
	attrset(A_NORMAL);
	
	if (status.bytes[2] & 0x10){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+27, "EOR");
	attrset(A_NORMAL);
	
	if (status.bytes[2] & 0x08){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+31, "IFAULT");
	attrset(A_NORMAL);
	
	if (status.bytes[2] & 0x04){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+38, "VSAG");
	attrset(A_NORMAL);
	
	if (status.bytes[3] & 0x80){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+43, "TUP");
	attrset(A_NORMAL);
	
	if (status.bytes[3] & 0x40){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+44, "TOD");
	attrset(A_NORMAL);
		
	if (status.bytes[3] & 0x10){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+48, "VOD");
	attrset(A_NORMAL);
	
		
	if (status.bytes[3] & 0x08){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+52, "IOD");
	attrset(A_NORMAL);
	
		
	if (status.bytes[3] & 0x04){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+56, "LSD");
	attrset(A_NORMAL);
	
		
	if (status.bytes[3] & 0x02){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+60, "FUP");
	attrset(A_NORMAL);
	
		
	if (status.bytes[3] & 0x01){
		attrset(A_REVERSE);
	}
	mvprintw(row,col1+64, "IC");
	attrset(A_NORMAL);	
}

void printMask(void) {
	Register countRegister = getRegister(26);
	unsigned int val = (((unsigned int)countRegister.bytes[1]) * 0x10000)+(((unsigned int)countRegister.bytes[2]) * 0x100)+(((unsigned int)countRegister.bytes[3]));
	mvprintw(bit_row, col4, "Status mask", val);
	mvprintw(bit_row, col4_v, "0x%06d", val);
}

void printConfiguration(void) {
	int row=bit_row+2;
	Register currentRegister = getRegister(0);
	
	unsigned int phaseCompensasion = (currentRegister.bytes[1] & 0xFE) > 1;
	if (phaseCompensasion & 0x40){
		phaseCompensasion = -(phaseCompensasion & 0x3F)+1;
	}
	
	unsigned gain = currentRegister.bytes[1] & 0x01;
	unsigned EWA = currentRegister.bytes[2] & 0x80;
	unsigned imode = (currentRegister.bytes[2] & 0x18) > 3;
	unsigned icpu =  (currentRegister.bytes[3] & 0x10) != 0;
	unsigned clockDivider = (currentRegister.bytes[3] & 0x0F);
	
	mvprintw(row, col1, "Phase Comp");
	mvprintw(row, col1_v, "%d", phaseCompensasion);
	mvprintw(row, col2, "gain");
	mvprintw(row, col2+5, "%d", gain);
	mvprintw(row, col2+8, "EWA");
	mvprintw(row, col2+12, "%d", EWA);
	mvprintw(row, col2+14, "icpu");
	mvprintw(row, col2+19, "%d", icpu);
	mvprintw(row, col3, "imode");
	switch(imode){
		case 0:
			mvprintw(row, col3_v, "active low");
			break;
		case 1:
			mvprintw(row, col3_v, "active hight");
			break;
		case 2:
			mvprintw(row, col3_v, "H to L pulse");
			break;
		case 3:
			mvprintw(row, col3_v, "L to H pulse");
			break;
	}
	mvprintw(row, col5, "Clock divider");
	mvprintw(row, col5_v, "%d", clockDivider);
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