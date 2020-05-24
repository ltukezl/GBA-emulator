#include <algorithm>
#include <iostream>
#include <fstream>
#include  <iomanip>
#include "MemoryOps.h"
#include "ThumbOpCodes.h"
#include "ARMOpCodes.h"
#include "Constants.h"
#include "GBAcpu.h"
#include "DMA.h"
#include "Display.h"
#include "interrupt.h"

#define GPU 1
#define BIOS_START 0

using namespace std;

bool debug = false;

/*Registers*/
/*prepare register ranges for banks*/
__int32 sharedRegs[9];
__int32 extRegisters[5];
__int32 usrBanked[2];
__int32 fiqBanked[7];
__int32 svcBanked[2];
__int32 abtBanked[2];
__int32 irqBanked[2];
__int32 undBanked[2];

__int32 sprs_usr = 0;
__int32 sprs_svc = 0;
__int32 sprs_abt = 0;
__int32 sprs_irq = 0;
__int32 sprs_udf = 0;

//__int32 r[16];	//used register

/*prepare complete banks for modes*/
__int32* usrSys[17] = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7], 
&extRegisters[0], &extRegisters[1], &extRegisters[2], &extRegisters[3], &extRegisters[4], &usrBanked[0], &usrBanked[1], &sharedRegs[8], &sprs_usr};

__int32* svc[17] = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7],
&extRegisters[0], &extRegisters[1], &extRegisters[2], &extRegisters[3], &extRegisters[4], &svcBanked[0], &svcBanked[1], &sharedRegs[8], &sprs_svc };

__int32* abt[17] = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7],
&extRegisters[0], &extRegisters[1], &extRegisters[2], &extRegisters[3], &extRegisters[4], &abtBanked[0], &abtBanked[1], &sharedRegs[8], &sprs_abt };

__int32* irq[17] = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7],
&extRegisters[0], &extRegisters[1], &extRegisters[2], &extRegisters[3], &extRegisters[4], &irqBanked[0], &irqBanked[1], &sharedRegs[8], &sprs_irq };

__int32* undef[17] = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7],
&extRegisters[0], &extRegisters[1], &extRegisters[2], &extRegisters[3], &extRegisters[4], &undBanked[0], &undBanked[1], &sharedRegs[8], &sprs_udf };

__int32** r;	//used register

//1  0  0  0
//N  Z  C  V  = sign,zero,carry,overflow
#if BIOS_START
__int32 cprs = 0xD3;	//current program status register
#else
__int32 cprs = 0x1f;	//current program status register
#endif

__int64 cycles = 0;
__int8 Wait0_N_cycles = 5;
__int8 Wait0_S_cycles = 3;


int swapEndianess32(int num){
	return ((num & 0xFF) << 24) + ((num & 0xFF00) << 8) + ((num & 0xFF0000) >> 8) + ((num & 0xFF000000) >> 24);
}


/*
NOTE *r[PC] = 0x08000000 can be used to skip bios check but needs to start in usr mode.
otherwise gba starts from addrs 0 in svc mode
*/
int main(int argc, char *args[]){

	IoRAM[0x130] = 0xFFFF; // input register, 0 = pressed down, 1 = released

#if GPU
	Display debugView(1024, 496, "paletteWindow");
	//Display gameDisplay(240, 160, "game");
#endif
	std::cout << *(int*)argc << "\n";

#if BIOS_START
	r = usrSys;
	*r[13] = SP_usr;
#else
	r = svc;
	*r[13] = 0x3007FE0;
#endif
	r = irq;
	*r[13] = SP_irq;
	


#if BIOS_START
	r = svc;
	*r[13] = SP_svc;
	*r[16] = 0x10;
#else
	r = usrSys;
	*r[13] = SP_usr;
#endif

#if BIOS_START
	*r[PC] = 0;
#else
	*r[PC] = 0x08000000;
#endif


#if BIOS_START
	
#else
	memoryLayout[4][6] = 0x9A; //Vcount initializtion use 7e when cycle counting is ready
#endif

    FILE *file;
	FILE* bios;
	fopen_s(&file, "program4.bin", "rb");
	fopen_s(&bios, "GBA.BIOS", "rb");
    //fopen_s(&file, args[1], "rb");
	fread(GamePak, 0x990000, 1, file);
	fread(systemROM, 0x3fff, 1, bios);

	
	int refreshRate = 0;

	while (true){
#if GPU
		if (debug | refreshRate > 10000)
			debugView.handleEvents();
#endif
		int thumbBit = (cprs >> 5) & 1;
		unsigned int opCode = loadFromAddress32(*r[PC], true);

		if (*r[15] == 0x3002890){
			//cout << "..";
			debug = true;
		}

		if (debug)
			cout << hex << *r[15] << " opCode: " << setfill('0') << setw(4) << (thumbBit ? opCode & 0xFFFF : opCode) << " ";

		thumbBit ? thumbExecute(opCode) : ARMExecute(opCode);
		startDMA();
		HWInterrupts(cycles);
#if GPU
		if (debug | refreshRate > 10000){
			debugView.updatePalettes();
			refreshRate = 0;
		}
#endif
		refreshRate++;
		//if (cycles >= 240){
		//	memoryLayout[4][6]++;
		//	cycles -= 240;
		//}
		if (debug){
			//std::cout << hex << *r[0] << " " << *r[1] << " " << *r[2] << " " << *r[3] << " " << *r[4] << " " << *r[5] << " " << *r[6] << " " << *r[7] << " " << *r[10] << " FP (r11): " << *r[11] << " IP (r12): " << *r[12] << " SP: " << *r[13] << " LR: " << *r[14] << " CPRS: " << cprs << " SPRS " << *r[16] << endl;
			std::cout << "cycles " << dec << cycles << std::endl;	
		}

	}
	std::cin >> *r[0];
    return 0;
}

