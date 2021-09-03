#include <math.h>
#include "Constants.h"
#include "MemoryOps.h"
#include "GBAcpu.h"
#include "armopcodes.h"
#include "iostream"
#include "timers.h"
#include <algorithm>
#include "memoryMappedIO.h"

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
uint32_t SP_usr = 0x03007F00;

//recheck waht should be done on unused regions, breath of fire writes to "unused" area
uint8_t systemROM[0x4000];
uint8_t ExternalWorkRAM[0x1000000];
uint8_t InternalWorkRAM[0x1000000]; //0x8000 should be in use only
uint8_t IoRAM[0x1000000];
uint8_t PaletteRAM[0x1000000];
uint8_t VRAM[0x1000000];
uint8_t OAM[0x1000000];
uint8_t GamePak[0x2000000];
uint8_t GamePakSRAM[0x2000000];

uint32_t memsizes[16] = { 0x4000, 0x4000, 0x40000, 0x2000000, 0x400, 0x400, 0x40000, 0x400, 0x2000000, 0x2000000, 0x2000000, 0x2000000, 0x2000000, 0x2000000, 0x2000000, 0x2000000 };
unsigned char *memoryLayout[16] = { systemROM, systemROM, ExternalWorkRAM, InternalWorkRAM, IoRAM, PaletteRAM, VRAM, OAM, GamePak, GamePak, GamePak, GamePak, GamePak, GamePak, GamePakSRAM, GamePakSRAM };

__int32 previousAddress = 0;

void rawWrite8(uint8_t* arr, uint32_t addr, uint8_t val){
	arr[addr] = val;
}

void rawWrite16(uint8_t* arr, uint32_t addr, uint16_t val){
	*(uint16_t*)&(uint8_t)arr[addr] = val;
}

void rawWrite32(uint8_t* arr, uint32_t addr, uint32_t val){
	*(uint32_t*)&(uint8_t)arr[addr] = val; 
}

uint8_t rawLoad8(uint8_t* arr, uint32_t addr){
	return arr[addr];
}

uint16_t rawLoad16(uint8_t* arr, uint32_t addr){
	return *(uint16_t*)&(uint8_t)arr[addr];
}

uint32_t rawLoad32(uint8_t* arr, uint32_t addr){
	return *(uint32_t*)&(uint8_t)arr[addr];
}

bool specialWrites(uint32_t addr, uint32_t val){
	if (addr >= 0x2040000 && addr <= 0x2FFFFFF){
		writeToAddress32(addr - 0x40000, val);
		return true;
	}
	else if (addr == 0x4000202) {//iinterrupt flag clear
		uint16_t tmp = loadFromAddress16(0x4000202, true);
		tmp &= ~val;
		rawWrite8(IoRAM, 0x202, tmp);
		return true;
	}
	else if (addr >= 0x03007F00 && addr <= 0x03007FFF) { //iknterrupt handler mirror
		writeToAddress32(addr | 0xFF8000, val);
		return false;
	}
	else if (addr >= 0x4000100 && addr <= 0x400010E){
		return timerReloadWrite(addr, val);
	}
	else if (addr >= 0x5000400 && addr <= 0x5FFFFFF){
		writeToAddress32(addr - 0x400, val);
		return true;
	}
	else if (addr >= 0x6020000 && addr <= 0x6FFFFFF){
		writeToAddress32(addr - 0x20000, val);
		return true;
	}
	else if (addr >= 0x7000400 && addr <= 0x7FFFFFF){
		writeToAddress32(addr - 0x4000, val);
		return true;
	}
	return false;
}

template<typename T, typename res>
bool specialReads(uint32_t addr, res& result, T func){
	if (addr >= 0x2040000 && addr <= 0x2FFFFFF){
		result = func(addr - 0x40000, true);
		return true;
	}
	else if (addr >= 0x3008000 && addr < 0x3FFFF00){
		result = func(addr - 0x8000, true);
		return true;
	}
	else if (addr >= 0x03007F00 && addr <= 0x03007FFF) {
		result = func(addr | 0xFF8000, true);
		return true;
	}
	else if (addr & ~(0xFF << 16) == 0x4000800){
		result = func(0x4000800, true);
		return true;
	}
	else if (addr >= 0x05000400 && addr <= 0x05FFFFFF) {
		result = func(addr - 0x400, true);
		return true;
	}
	else if (addr >= 0x06018000 && addr < 0x06020000) {
		result = func(addr - 0x18000, true);
		return true;
	}
	else if (addr >= 0x06020000 && addr <= 0x06040000) {
		result = func(addr - 0x20000, true);
		return true;
	}
	else if (addr >= 0x07000400 && addr <= 0x07FFFFFF) {
		result = func(addr - 0x400, true);
		return true;
	}
	else if (addr >= 0x0c000000){
		result = func(addr - 0x4000000, true);
		return true;
	}
	else if (addr >= 0x0A000000){
		result = func(addr - 0x2000000, true);
		return true;
	}
	return false;
}

void writeToAddress(uint32_t address, uint8_t value){
	address &= ~0xF0000000;
    int mask = (address >> 24) & 15;
	if (!specialWrites(address, value) & address < 0x7000000){
		memoryLayout[mask][address - (mask << 24)] = value;
	}
}

void writeToAddress16(uint32_t address, uint16_t value){
	address &= ~0xF0000000;
	int mask = (address >> 24) & 15;
	if (!specialWrites(address, value)){
		*(uint16_t*)&(uint8_t)memoryLayout[mask][address - (mask << 24)] = value;
	}
}

void writeToAddress32(uint32_t address, uint32_t value){
	address &= ~0xF0000000;
	address &= ~1;
	int mask = (address >> 24) & 15;
	if (!specialWrites(address, value)){
		*(uint32_t*)&(uint8_t)memoryLayout[mask][address - (mask << 24)] = value;
	}
	if (address == 0x4000208){ //waitstate reg

	}
}

uint8_t loadFromAddress(uint32_t address, bool free){
	if (!free){
		cycles += Wait0_N_cycles;
		if (address == (previousAddress + 1))
			cycles += Wait0_S_cycles;
		else
			cycles += Wait0_N_cycles;
		previousAddress = address;
	}

	address &= ~0xF0000000;
    int mask = (address >> 24) & 15;

	uint8_t result = 0;
	if (specialReads(address, result, loadFromAddress)){
		return result;
	}
	return memoryLayout[mask][address - (mask << 24)];
}

uint16_t loadFromAddress16(uint32_t address, bool free){
	if (!free){
		cycles += Wait0_N_cycles;
		if (address == (previousAddress + 2))
			cycles += Wait0_S_cycles;
		else
			cycles += Wait0_N_cycles;
		previousAddress = address;
	}

	address &= ~0xF0000000;
	int mask = (address >> 24) & 15;

	uint16_t result = 0;
	if (specialReads(address, result, loadFromAddress16)){
		return result;
	}

	return *(uint16_t*)&(uint8_t)memoryLayout[mask][address - (mask << 24) ];
}

uint32_t loadFromAddress32(uint32_t address, bool free){
	if (!free){
		cycles += Wait0_N_cycles;
		if (address == (previousAddress + 4))
			cycles += Wait0_S_cycles;
		else
			cycles += Wait0_N_cycles;
		previousAddress = address;
	}
	
	bool misaligned = address & 1;
	address &= ~0xF0000000;
	address &= ~1;
    int mask = (address >> 24) & 15;

	uint32_t result = 0;
	if (specialReads(address, result, loadFromAddress32)){
		return result;
	}
	if (misaligned)
		return RORnoCond(*(uint32_t*)&(uint8_t)memoryLayout[mask][address - (mask << 24) + 0], 8);
	return *(uint32_t*)&(uint8_t)memoryLayout[mask][address - (mask << 24)];
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

void memoryInits(){
	writeToAddress32(0x4000800, 0x0D000020);
}
