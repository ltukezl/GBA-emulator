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

uint8_t systemROM[0x4000] = { 0 };
uint8_t ExternalWorkRAM[0x40000] = { 0 };
uint8_t InternalWorkRAM[0x8000] = { 0 };
uint8_t IoRAM[0x400] = { 0 };
uint8_t PaletteRAM[0x400] = { 0 };
uint8_t VRAM[0x18000] = { 0 };
uint8_t OAM[0x400] = { 0 };
uint8_t GamePak[0x2000000] = { 0 };
uint8_t GamePakSRAM[0x10000] = { 0 };

uint32_t memsizes[16] = { 0x4000, 0x4000, 0x40000, 0x8000, 0x400, 0x400, 0x20000, 0x400, 0x1000000, 0x1000000, 0x1000000, 0x1000000, 0x1000000, 0x1000000, 0x10000, 0x10000 };
unsigned char *memoryLayout[16] = { systemROM, systemROM, ExternalWorkRAM, InternalWorkRAM, IoRAM, PaletteRAM, VRAM, OAM, GamePak, &GamePak[0x1000000], GamePak, &GamePak[0x1000000], GamePak, &GamePak[0x1000000], GamePakSRAM, GamePakSRAM };

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

bool specialWrites(uint32_t mask, uint32_t addr, uint32_t val){
	if (mask == 4 && addr == 0x202) {//iinterrupt flag clear
		uint16_t tmp = rawLoad16(IoRAM, 0x202);
		tmp &= ~val;
		rawWrite16(IoRAM, 0x202, tmp);
		return true;
	}
	else if (addr >= 0x4000100 && addr <= 0x400010E){
		return timerReloadWrite(addr, val);
	}

	return false;
}

template<typename T, typename res>
bool specialReads(uint32_t addr, res& result, T func){
	if (addr >= 0x2040000 && addr <= 0x2FFFFFF){
		//result = func(addr - 0x40000, true);
		//return true;
	}
	return false;
}

void writeToAddress(uint32_t address, uint8_t value){
	int mask = (address >> 24) & 15;
	address &= ~0xFF000000;

	if (mask == 7 && value == 0xd8)
		debug = false;

	if (mask == 4 && address > memsizes[mask])
		return;

	if (mask == 6 && displayCtrl->bgMode == 0)
		address %= 0x8000;
	else
		address %= memsizes[mask];

	if (mask == 7 
		|| (displayCtrl->bgMode == 7 && mask == 6 && address >= 0x10000)
		|| (displayCtrl->bgMode == 4 && mask == 6 && address >= 0x10000))
		return;

	if ((displayCtrl->bgMode == 4 && mask == 6 && address < 0x10000)
		|| mask == 5){
		memoryLayout[mask][address] = value;
		memoryLayout[mask][address + 1] = value;
		return;
	}

	if (specialWrites(mask, address, value))
		return;

	memoryLayout[mask][address] = value;
}

void writeToAddress16(uint32_t address, uint16_t value){
	int mask = (address >> 24) & 15;
	address &= ~0xFF000000;
	uint32_t misalignment = address & 1;

	if (mask == 4 && address > memsizes[mask])
		return;

	if (mask == 6 && displayCtrl->bgMode == 0)
		address %= 0x8000;
	else
		address %= memsizes[mask];

	if (specialWrites(mask, address, value))
		return;

	*(uint16_t*)&(uint8_t)memoryLayout[mask][address - misalignment] = value;
}

void writeToAddress32(uint32_t address, uint32_t value){
	int mask = (address >> 24) & 15;
	address &= ~0xFF000000;
	uint32_t misalignment = address & 3;

	if (mask == 4 && address > memsizes[mask])
		return;

	if (mask == 6 && displayCtrl->bgMode == 0)
		address %= 0x8000;
	else
		address %= memsizes[mask];

	if (specialWrites(mask, address, value))
		return;

	*(uint32_t*)&(uint8_t)memoryLayout[mask][address - misalignment] = value;
	
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

	int mask = (address >> 24) & 15;
	address &= ~0xFF000000;

	if (mask == 4 && address > memsizes[mask])
		return 0;

	if (mask == 6 && displayCtrl->bgMode == 0)
		address %= 0x8000;
	else
		address %= memsizes[mask];

	return memoryLayout[mask][address];
}

uint32_t loadFromAddress16(uint32_t address, bool free){
	if (!free){
		cycles += Wait0_N_cycles;
		if (address == (previousAddress + 2))
			cycles += Wait0_S_cycles;
		else
			cycles += Wait0_N_cycles;
		previousAddress = address;
	}
	bool misaligned = address & 1;
	int mask = (address >> 24) & 15;
	address &= ~0xFF000000;

	if (mask == 4 && address > memsizes[mask])
		return 0;

	if (mask == 6 && displayCtrl->bgMode == 0)
		address %= 0x8000;
	else
		address %= memsizes[mask];

	if (misaligned)
		return RORnoCond(*(uint16_t*)&(uint8_t)memoryLayout[mask][(address - 1)], 8);
	return *(uint16_t*)&(uint8_t)memoryLayout[mask][address] & 0xFFFF;
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

	uint32_t misalignment = address & 3;
    uint32_t mask = (address >> 24) & 15;
	address &= ~0xFF000000;

	if (mask == 4 && address > memsizes[mask])
		return 0;

	if (mask == 6 && displayCtrl->bgMode == 0)
		address %= 0x8000;
	else
		address %= memsizes[mask];

	return RORnoCond(rawLoad32(memoryLayout[mask], address - misalignment), (8 * misalignment));
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
	//writeToAddress32(0, 0xe129f000);
	rawWrite32(IoRAM, 0x800, 0x0D000020);
}
