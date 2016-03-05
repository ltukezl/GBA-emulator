#include <algorithm>
#include <iostream>
#include <fstream>
#include "MemoryOps.h"
#include "ThumbOpCodes.h"
#include "ARMOpCodes.h"
#include "Constants.h"
#include "GBAcpu.h"

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

/*prepare complete banks for modes*/
__int32* usrSys[16] = { &sharedRegs[0], &sharedRegs[1], &sharedRegs[2], &sharedRegs[3], &sharedRegs[4], &sharedRegs[5], &sharedRegs[6], &sharedRegs[7], 
&extRegisters[0], &extRegisters[1], &extRegisters[2], &extRegisters[3], &extRegisters[4], &usrBanked[0], &usrBanked[1], &sharedRegs[8]};
__int32 fiq[16];
__int32 sup[16];
__int32 abt[16];
__int32 irq[16];
__int32 undef[16];


__int32 r[16];	//used register
__int32 &SP = r[13];	//stack pointer register
__int32 &LR = r[14];	//link register
__int32 &PC = r[15]; 	//program counter (r15)
//1  0  0  0
//N  Z  C  V  = sign,zero,carry,overflow
__int32 cprs = 0;	//current program status register

int swapEndianess32(int num){
	return ((num & 0xFF) << 24) + ((num & 0xFF00) << 8) + ((num & 0xFF0000) >> 8) + ((num & 0xFF000000) >> 24);
}

int main(){
    FILE *file;
    fopen_s(&file, "program.bin", "rb");

    fread(GamePak, 50000, 1, file);

    SP = StackStart;
	PC = 0x08000000;

	/*
    while (GamePak[PC] != 0 || GamePak[PC + 1] != 0){
        PC += 2; //4 bytes at time on thumb instructions
        thumbExecute((GamePak[PC - 1] << 8) + GamePak[PC - 2]); // swap endianess
        cout << "opCode: " << hex << ((GamePak[PC - 1] << 8) + GamePak[PC - 2]) << " ";
        std::cout <<  r[0] << " " << r[1] << " " << r[2] << " " << r[3] << " " <<  r[4] << " " << r[5] << " " << r[6] << " " << r[7] << " " << r[13] <<" " << r[14] <<" " << r[15] <<  "\n";
    }
	*/

	while (loadFromAddress32(PC)){
		int thumbBit = (cprs >> 5) & 1;
		PC = thumbBit ? PC + 2: PC + 4;
		int opCode = thumbBit ? loadFromAddress16(PC - 2) & 0xFFFF : loadFromAddress32(PC - 4);
		cout << "opCode: " << hex << opCode << " ";
		thumbBit ? thumbExecute(loadFromAddress16(PC - 2)) : ARMExecute(loadFromAddress32(PC - 4));
		std::cout << r[0] << " " << r[1] << " " << r[2] << " " << r[3] << " " << r[4] << " " << r[5] << " " << r[6] << " " << r[7] << " FP: " << r[11] << " IP: " << r[12] << " SP: " << r[13] << " LR: " << r[14] << " PC: " << r[15] - 4 << " CPRS: " << cprs << "\n";
	}

	std::cin >> r[0];
    return 0;
}

