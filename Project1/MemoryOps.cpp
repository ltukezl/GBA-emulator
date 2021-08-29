#include <math.h>
#include "Constants.h"
#include "MemoryOps.h"
#include "GBAcpu.h"
#include "iostream"
#include "timers.h"
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

__int32 previousAddress = 0;

void intWrite(uint16_t value){
	*(unsigned short*)&(unsigned char)memoryLayout[4][0x202] = value;
}

bool specialWrites(uint32_t addr, uint32_t val){
	if (addr == 0x4000202) {//iinterrupt flag clear
		uint16_t tmp = loadFromAddress16(0x4000202, true);
		tmp &= ~val;
		intWrite(tmp);
		return true;
	}
	if (addr == 0x03007FFC) { //iknterrupt handler mirror
		writeToAddress32(0x3FFFFFC, val);
		return false;
	}
	else if (addr >= 0x4000100 && addr <= 0x400010E){
		return timerReloadWrite(addr, val);
	}
	return false;
}

uint32_t specialReads(uint32_t addr){
	if (addr == 0x03007FFC) {
		return loadFromAddress32(0x3FFFFFC);
	}
}

void writeToAddress(uint32_t address, uint8_t value){
	address &= ~0xF0000000;
    int mask = (address >> 24) & 15;
	if (!specialWrites(address, value)){
		memoryLayout[mask][address - (mask << 24)] = value;
	}
}

void writeToAddress16(uint32_t address, uint16_t value){
	address &= ~0xF0000000;
	int mask = (address >> 24) & 15;
	if (!specialWrites(address, value)){
		*(unsigned short*)&(unsigned char)memoryLayout[mask][address - (mask << 24) + 0] = value;
	}
}

void writeToAddress32(uint32_t address, uint32_t value){
	address &= ~0xF0000000;
	int mask = (address >> 24) & 15;
	if (!specialWrites(address, value)){
		*(unsigned int*)&(unsigned char)memoryLayout[mask][address - (mask << 24) + 0] = value;
	}
	if (address == 0x4000208){ //waitstate reg

	}
}

uint8_t loadFromAddress(uint32_t address, bool free){
	address &= ~0xF0000000;
    int mask = (address >> 24) & 15;

	if (!free){
		cycles += Wait0_N_cycles;
		if (address == (previousAddress + 1))
			cycles += Wait0_S_cycles;
		else
			cycles += Wait0_N_cycles;
		previousAddress = address;
	}

	return memoryLayout[mask][address - (mask << 24)];
}

uint16_t loadFromAddress16(uint32_t address, bool free){
	address &= ~0xF0000000;
	int mask = (address >> 24) & 15;
	int number = *(unsigned short*)&(unsigned char)memoryLayout[mask][address - (mask << 24) + 0];

	if (!free){
		cycles += Wait0_N_cycles;
		if (address == (previousAddress + 2))
			cycles += Wait0_S_cycles;
		else
			cycles += Wait0_N_cycles;
		previousAddress = address;
	}

	return number;
}

uint32_t loadFromAddress32(uint32_t address, bool free){
	address &= ~0xF0000000;
    int mask = (address >> 24) & 15;
	int number = *(unsigned int*)&(unsigned char)memoryLayout[mask][address - (mask << 24) + 0];

	if (!free){
		cycles += Wait0_N_cycles;
		if (address == (previousAddress + 4))
			cycles += Wait0_S_cycles;
		else
			cycles += Wait0_N_cycles;
		previousAddress = address;
	}

	return number;
}

void PUSH(int value){
    *r[SP] -= 4;
	writeToAddress32(*r[SP], value);
}

unsigned __int32 POP(){

    int value = loadFromAddress32(*r[SP]);
    *r[SP] += 4;
    return value;
}
