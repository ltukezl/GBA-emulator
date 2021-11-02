#include "Constants.h"
#include "MemoryOps.h"
#include "GBAcpu.h"
#include "Arm/armopcodes.h"
#include "iostream"
#include "Timer/timers.h"
#include "memoryMappedIO.h"
#include "Display/Display.h"

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
uint8_t IoRAM[0x801] = { 0 };
uint8_t PaletteRAM[0x400] = { 0 };
uint8_t VRAM[0x18000] = { 0 };
uint8_t OAM[0x400] = { 0 };
uint8_t GamePak[0x2000000] = { 0 };
uint8_t GamePakSRAM[0x10000] = { 0 };

uint32_t memsizes[16] = { 0x4000, 0x4000, 0x40000, 0x8000, 0x400, 0x400, 0x20000, 0x400, 0x1000000, 0x1000000, 0x1000000, 0x1000000, 0x1000000, 0x1000000, 0x10000, 0x10000 };
unsigned char *memoryLayout[16] = { systemROM, systemROM, ExternalWorkRAM, InternalWorkRAM, IoRAM, PaletteRAM, VRAM, OAM, GamePak, &GamePak[0x1000000], GamePak, &GamePak[0x1000000], GamePak, &GamePak[0x1000000], GamePakSRAM, GamePakSRAM };

uint32_t previousAddress = 0;

void calculateCycles(uint32_t address, bool isSequental){
	if (isSequental){
		if (address >= 0xC000000){
			cycles += WS2Second[waitStateControl->waitstate2Second];
		}
		else if (address >= 0xA000000){
			cycles += WS1Second[waitStateControl->waitstate2Second];
		}
		else if (address >= 0x8000000){
			cycles += WS0Second[waitStateControl->waitstate2Second];
		}
	}
	else{
		if (address >= 0xC000000){
			cycles += firstAccessCycles[waitStateControl->waitstate2First];
		}
		else if (address >= 0xA000000){
			cycles += firstAccessCycles[waitStateControl->waitstate1First];
		}
		else if (address >= 0x8000000){
			cycles += firstAccessCycles[waitStateControl->waitstate0First];
		}
	}
	previousAddress = address;
}

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
	else if (mask == 4 && (addr == 0x100 || addr == 0x104 || addr == 0x108 || addr == 0x10B)){
		return timerReloadWrite(addr, val);
	}
	else if (mask == 4 && (addr == 0x102 || addr == 0x106 || addr == 0x10A || addr == 0x10D)){
		return reloadCounter(addr, val);
	}
	else if (mask == 6 && addr < 0x10000){
		debugView->VRAMupdated = true;
	}
	else if (mask == 6 && addr >= 0x10000){
		debugView->OBJupdated = true;
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

uint32_t clampAddress(uint32_t mask, uint32_t address){
	if (mask == 5 && address == 2)
		debug = false;
	address %= memsizes[mask];
	if (mask == 6 && address >= 0x18000)
		address -= 0x8000;

	return address;
}

void writeToAddress(uint32_t address, uint8_t value){
	calculateCycles(address, (previousAddress + 1) == address);
	int mask = (address >> 24) & 15;
	address &= ~0xFF000000;

	if (mask == 4 && address > memsizes[mask])
		return;

	else if ((mask == 5 || mask == 6 || mask == 7) & (displayCtrl->forceBlank || InterruptFlagRegister->vBlank || InterruptFlagRegister->hBlank))
		return;

	else if (mask == 0)
		return;

	address = clampAddress(mask, address);

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
	calculateCycles(address, (previousAddress + 2) == address);
	int mask = (address >> 24) & 15;
	address &= ~0xFF000000;
	uint32_t misalignment = address & 1;

	if (mask == 4 && address > memsizes[mask])
		return;
	else if (mask == 0)
		return;

	address = clampAddress(mask, address);

	if (specialWrites(mask, address, value))
		return;

	*(uint16_t*)&(uint8_t)memoryLayout[mask][address - misalignment] = value;
}

void writeToAddress32(uint32_t address, uint32_t value){
	calculateCycles(address, (previousAddress + 4) == address);
	calculateCycles(address, true);
	cycles += 1;
	int mask = (address >> 24) & 15;
	address &= ~0xFF000000;
	uint32_t misalignment = address & 3;

	if (mask == 4 && address > memsizes[mask])
		return;

	else if (mask == 0)
		return;

	address = clampAddress(mask, address);

	if (specialWrites(mask, address, value))
		return;

	*(uint32_t*)&(uint8_t)memoryLayout[mask][address - misalignment] = value;
}

uint8_t loadFromAddress(uint32_t address, bool free){
	if (!free)
		calculateCycles(address, (previousAddress + 1) == address);

	int mask = (address >> 24) & 15;
	address &= ~0xFF000000;

	if (mask == 4 && address > memsizes[mask])
		return 0;

	address = clampAddress(mask, address);

	return memoryLayout[mask][address];
}

uint32_t loadFromAddress16(uint32_t address, bool free){
	if (!free)
		calculateCycles(address, (previousAddress + 2) == address);

	bool misaligned = address & 1;
	int mask = (address >> 24) & 15;
	address &= ~0xFF000000;

	if (mask == 4 && address > memsizes[mask])
		return 0;

	address = clampAddress(mask, address);

	if (misaligned)
		return RORnoCond(*(uint16_t*)&(uint8_t)memoryLayout[mask][(address - 1)], 8);
	return *(uint16_t*)&(uint8_t)memoryLayout[mask][address] & 0xFFFF;
}

uint32_t loadFromAddress32(uint32_t address, bool free){
	if (!free){
		calculateCycles(address, (previousAddress + 4) == address);
		calculateCycles(address, true);
		cycles += 1;
	}

	uint32_t misalignment = address & 3;
    uint32_t mask = (address >> 24) & 15;
	address &= ~0xFF000000;

	if (mask == 4 && address > memsizes[mask])
		return 0;

	address = clampAddress(mask, address);

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