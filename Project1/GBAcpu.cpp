#include <iostream>
#include <fstream>
#include "MemoryOps.h"
#include "ThumbOpCodes.h"
#include "ARMOpCodes.h"
#include "Constants.h"
#include "GBAcpu.h"

using namespace std;

/*Registers*/
__int32 r[16];	//general purpose registers
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

    fread(GamePak, 500, 1, file);

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
		PC += 4;
		cout << "opCode: " << hex << loadFromAddress32(PC - 4) << " ";
		ARMExecute(loadFromAddress32(PC - 4));
		std::cout << r[0] << " " << r[1] << " " << r[2] << " " << r[3] << " " << r[4] << " " << r[5] << " " << r[6] << " " << r[7] << " FP: " << r[11] << " IP: " << r[12] << " SP: " << r[13] << " LR: " << r[14] << " PC: " << r[15]-4 << "\n";
	}

	std::cin >> r[0];
    return 0;
}

