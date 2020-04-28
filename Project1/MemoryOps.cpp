#include <math.h>
#include "Constants.h"
#include "MemoryOps.h"
#include "GBAcpu.h"
#include "iostream"
#include <algorithm>

uint32_t systemROMStart = 0x00000000;
uint32_t ExternalWorkRAMStart = 0x02000000;
uint32_t InternalWorkRAMStart = 0x03000000;
uint32_t IoRAMStart = 0x04000000;
uint32_t PaletteRAMStart = 0x05000000;
uint32_t VRAMStart = 0x06000000;
uint32_t OAMStart = 0x07000000;
uint32_t GamePakStart = 0x08000000;
uint32_t SP_svc = 0x03007F00;
uint32_t SP_irq = 0x03007FA0;
uint32_t SP_usr = 0x03007FEA;

//recheck waht should be done on unused regions, breath of fire writes to "unused" area
unsigned char systemROM[0x4000];
unsigned char unused[0x4000];
unsigned char ExternalWorkRAM[0x1000000];
unsigned char InternalWorkRAM[0x1000000]; //0x8000 should be in use only
unsigned char IoRAM[0x1000000];
unsigned char PaletteRAM[0x1000000];
unsigned char VRAM[0x1000000];
unsigned char OAM[0x1000000];
unsigned char GamePak[0x2000000];
unsigned char GamePakSRAM[0x2000000];

unsigned char *memoryLayout[16] = { systemROM, unused, ExternalWorkRAM, InternalWorkRAM, IoRAM, PaletteRAM, VRAM, OAM, GamePak, GamePak, GamePak, GamePak, GamePak, GamePak, GamePakSRAM, GamePakSRAM };

void writeToAddress(int address, int value){
	address &= ~0xF0000000;
    int mask = (address >> 24) & 15;
	memoryLayout[mask][address - (mask << 24)] = value;
}

int loadFromAddress(int address){
	address &= ~0xF0000000;
    int mask = (address >> 24) & 15;
	return memoryLayout[mask][address - (mask << 24)];
}
//could propably do faster with *(int*) but might not work in different endianess
void writeToAddress32(int address, int value){
	address &= ~0xF0000000;
    int mask = (address >> 24) & 15;
	//std::cout << "writetoaddress32 " << address - (mask << 24) << " value " << value << std::endl;
	*(unsigned int*)&(unsigned char)memoryLayout[mask][address - (mask << 24) + 0] = value;
	
	/*
	memoryLayout[mask][address - (mask << 24) + 0] = value & 0xFF;
	memoryLayout[mask][address - (mask << 24) + 1] = (value >> 8) & 0xFF;
	memoryLayout[mask][address - (mask << 24) + 2] = (value >> 16) & 0xFF;
	memoryLayout[mask][address - (mask << 24) + 3] = (value >> 24) & 0xFF;
	*/
}

unsigned __int32 loadFromAddress32(int address){
	address &= ~0xF0000000;
    int mask = (address >> 24) & 15;
	int number = *(unsigned int*)&(unsigned char)memoryLayout[mask][address - (mask << 24) + 0];
	/*
	int number = 0;
	number |= (unsigned char)memoryLayout[mask][address - (mask << 24) + 3] << 24;
	number |= (unsigned char)memoryLayout[mask][address - (mask << 24) + 2] << 16;
	number |= (unsigned char)memoryLayout[mask][address - (mask << 24) + 1] << 8;
	number |= (unsigned char)memoryLayout[mask][address - (mask << 24) + 0];
	//std::cout << "loadfromaddress32 " << address << " " << number << " " << address << std::endl;
	*/
	return number;
}

void writeToAddress16(int address, int value){
	address &= ~0xF0000000;
    int mask = (address >> 24) & 15;
	*(unsigned short*)&(unsigned char)memoryLayout[mask][address - (mask << 24) + 0] = value;
	/*
	memoryLayout[mask][address - (mask << 24) + 0] = value & 0xFF;
	memoryLayout[mask][address - (mask << 24) + 1] = (value >> 8) & 0xFF;
	*/
}

unsigned __int16 loadFromAddress16(int address){
	address &= ~0xF0000000;
    int mask = (address >> 24) & 15;
	int number = *(unsigned short*)&(unsigned char)memoryLayout[mask][address - (mask << 24) + 0];
	/*
	number |= (unsigned char)memoryLayout[mask][address - (mask << 24) + 1] << 8;
	number |= (unsigned char)memoryLayout[mask][address - (mask << 24) + 0];
	//std::cout << "loadfromaddress32 " << address << " " << number << " " << address << std::endl;
	*/
	return number;
}

void PUSH(int value){
    *r[SP] -= 4;
    //std::cout << "Pushed " << value <<" to "<< *r[SP] <<"\n";
	writeToAddress32(*r[SP], value);
}

unsigned __int32 POP(){

    int value = loadFromAddress32(*r[SP]);
    //std::cout << "Popped " << value <<" from "<< *r[SP] <<" to ";
    *r[SP] += 4;
    return value;
}
