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

uint8_t systemROM[0x4000] = {};
uint8_t ExternalWorkRAM[0x40000] = {};
uint8_t InternalWorkRAM[0x8000] = {};
uint8_t IoRAM[0x801] = {};
uint8_t PaletteRAM[0x400] = {};
uint8_t VRAM[0x18000] = {};
uint8_t OAM[0x400] = {};
uint8_t* GamePak;
uint8_t GamePakSRAM[0x10000] = {};

const uint32_t memsizes[16] = { 0x4000, 0x4000, 0x40000, 0x8000, 0x400, 0x400, 0x20000, 0x400, 0x1000000, 0x1000000, 0x1000000, 0x1000000, 0x1000000, 0x1000000, 0x10000, 0x10000 };
unsigned char *memoryLayout[16] = { systemROM, systemROM, ExternalWorkRAM, InternalWorkRAM, IoRAM, PaletteRAM, VRAM, OAM, GamePak, &GamePak[0x1000000], GamePak, &GamePak[0x1000000], GamePak, &GamePak[0x1000000], GamePakSRAM, GamePakSRAM };

uint32_t previousAddress = 0;
extern RgbaPalette PaletteColours;

bool isInterrupt() {
	return cpsr.mode == SUPER || cpsr.mode == IRQ;
}

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

void isVideoMemModification(uint32_t addr) {
	PaletteColours.paletteMemChanged(addr);
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
	else if (mask == 5) {
		debugView->VRAMupdated = true;
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
	address %= memsizes[mask];
	if (mask == 6 && address >= 0x18000)
		address -= 0x8000;

	return address;
}

void DmaIncreasing(uint32_t destination, uint32_t source, uint32_t size) {
	isVideoMemModification(destination);
	destination &= ~3;
	source &= ~3;

	uint32_t mask1 = (destination >> 24) & 15;
	uint32_t mask2 = (source >> 24) & 15;

	if (mask1 >= 8 && mask1 < 0xD)
		return;
	if (mask1 == 0)
		return;

	destination = clampAddress(mask1, destination);
	source = clampAddress(mask2, source);

	memcpy(&memoryLayout[mask1][destination], &memoryLayout[mask2][source], size * 4);
}

void writeToAddress(uint32_t address, uint8_t value){
	isVideoMemModification(address);
	calculateCycles(address, (previousAddress + 1) == address);
	int mask = (address >> 24) & 15;
	address &= ~0xFF000000;

	if (mask == 4 && address > memsizes[mask])
		return;

	else if ((mask == 5 || mask == 6 || mask == 7) && (displayCtrl->forceBlank || InterruptFlagRegister->vBlank || InterruptFlagRegister->hBlank))
		return;

	else if (mask == 0)
		return;

	else if (mask >= 8 && mask <= 0xd)
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
	isVideoMemModification(address);
	calculateCycles(address, (previousAddress + 2) == address);
	int mask = (address >> 24) & 15;
	address &= ~0xFF000000;
	uint32_t misalignment = address & 1;

	if (mask == 4 && address > memsizes[mask])
		return;
	else if (mask == 0)
		return;
	else if (mask >= 8 && mask <= 0xd)
		return;
	else if (mask == 0xe)
	{
		//sram writes are 8 bits
		value = ((value & 0xFF) << 8) | (value & 0xFF);
	}

	address = clampAddress(mask, address);

	if (specialWrites(mask, address, value))
		return;

	*(uint16_t*)&(uint8_t)memoryLayout[mask][address - misalignment] = value;
}

void writeToAddress32(uint32_t address, uint32_t value){
	isVideoMemModification(address);
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

	else if (mask >= 8 && mask <= 0xd)
		return;
	else if (mask == 0xe)
	{
		//sram writes are 8 bits
		value = ((value & 0xFF) << 24) | ((value & 0xFF) << 16) | ((value & 0xFF) << 8) | (value & 0xFF);
	}

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

	uint32_t misalignment = address & 1;

	int mask = (address >> 24) & 15;
	address &= ~0xFF000000;

	if (mask == 4 && address > memsizes[mask])
		return 0;

	address = clampAddress(mask, address);
	uint16_t tmpResult = rawLoad16(memoryLayout[mask], (address - misalignment));
	if (mask == 0xe)
	{
		//sram writes are 8 bits
		tmpResult = ((tmpResult & 0xFF) << 8) | (tmpResult & 0xFF);
	}

	return RORnoCond(tmpResult, 8 * misalignment);
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

	uint32_t result = 0;

	if (r[15] < 0x4000)
		result = rawLoad32(memoryLayout[mask], address - misalignment);
	else
		result = RORnoCond(rawLoad32(memoryLayout[mask], address - misalignment), (8 * misalignment));

	if (mask == 0xe)
	{
		//sram writes are 8 bits
		result = ((result & 0xFF) << 24) | ((result & 0xFF) << 16) | ((result & 0xFF) << 8) | (result & 0xFF);
	}

	return result;
}

void PUSH(int value){
    r[SP] -= 4;
	writeToAddress32(r[SP], value);
}

unsigned __int32 POP(){

    int value = loadFromAddress32(r[SP]);
    r[SP] += 4;
    return value;
}

void memoryInits(){
	//writeToAddress32(0, 0xe129f000);
	rawWrite32(IoRAM, 0x800, 0x0D000020);
	GamePak = new uint8_t[0x2000000];
	memset(GamePak, 0, sizeof(uint8_t) * 0x2000000);
	memoryLayout[8] = GamePak;
	memoryLayout[9] = &GamePak[0x1000000];
	memoryLayout[10] = GamePak;
	memoryLayout[11] = &GamePak[0x1000000];
	memoryLayout[12] = GamePak;
	memoryLayout[13] = &GamePak[0x1000000];
}
