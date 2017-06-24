#include <algorithm>
#include <iostream>
#include <fstream>
#include "MemoryOps.h"
#include "ThumbOpCodes.h"
#include "ARMOpCodes.h"
#include "Constants.h"
#include "GBAcpu.h"
#include "DMA.h"
#include "Display.h"

#define GPU

using namespace std;

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
__int32 cprs = 0x1f;	//current program status register

int swapEndianess32(int num){
	return ((num & 0xFF) << 24) + ((num & 0xFF00) << 8) + ((num & 0xFF0000) >> 8) + ((num & 0xFF000000) >> 24);
}


/*
NOTE *r[PC] = 0x08000000 can be used to skip bios check but needs to start in usr mode.
otherwise gba starts from addrs 0 in svc mode
*/
int main(int argc, char *args[]){
#ifdef GPU
	Display palettes(256, 496, "paletteWindow");
	Display mainDisplay(240, 160, "main window");
#endif
	std::cout << *(int*)argc << "\n";

	r = svc;
	*r[13] = SP_svc;
	r = irq;
	*r[13] = SP_irq;
	*r[16] = 0x10;
	r = usrSys;
	*r[13] = SP_usr;

	*r[PC] = 0x8000000; //pc

    FILE *file;
	FILE* bios;
	fopen_s(&file, "program.bin", "rb");
	fopen_s(&bios, "GBA.BIOS", "rb");
    //fopen_s(&file, args[1], "rb");
	fread(GamePak, 0x990000, 1, file);
	fread(systemROM, 0x3fff, 1, bios);
	while (true){
#ifdef GPU
		mainDisplay.handleEvents();
#endif
		int thumbBit = (cprs >> 5) & 1;
		unsigned int opCode = thumbBit ? loadFromAddress16(*r[PC]) : loadFromAddress32(*r[PC]);
		if (*r[PC] == 0xb4c){
			cout << "..";
		}
		cout << hex << *r[15] << " opCode: " << opCode << " ";
		thumbBit ? thumbExecute(loadFromAddress16(*r[PC])) : ARMExecute(loadFromAddress32(*r[PC]));
#ifdef GPU
		startDMA();
		palettes.updatePalettes();
		mainDisplay.updateStack();
#endif
		std::cout << *r[0] << " " << *r[1] << " " << *r[2] << " " << *r[3] << " " << *r[4] << " " << *r[5] << " " << *r[6] << " " << *r[7] << " FP (r11): " << *r[11] << " IP (r12): " << *r[12] << " SP: " << *r[13] << " LR: " << *r[14] << " CPRS: " << cprs << " SPRS " << *r[16] <<  "\n" ;
	}

	std::cin >> *r[0];
    return 0;
}

