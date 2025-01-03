#include "Constants.h"
#include "memory/memoryOps.h"
#include "GBAcpu.h"
#include "Arm/armopcodes.h"
#include "iostream"
#include "Timer/timers.h"
#include "Memory/memoryMappedIO.h"
#include "Display/Display.h"

uint32_t systemROMStart = 0x00000000;
uint32_t IoRAMStart = 0x04000000;
uint32_t VRAMStart = 0x06000000;
uint32_t OAMStart = 0x07000000;
uint32_t GamePakStart = 0x08000000;
uint32_t SP_svc = 0x03007F00;
uint32_t SP_irq = 0x03007FA0;
uint32_t SP_usr = 0x03007F00;

const uint32_t memsizes[16] = { 0x4000, 0x4000, 0x40000, 0x8000, 0x400, 0x400, 0x20000, 0x400, 0x1000000, 0x1000000, 0x1000000, 0x1000000, 0x1000000, 0x1000000, 0x400000, 0x400000 };

uint32_t previousAddress = 0;
extern RgbaPalette PaletteColours;

uint8_t IoRAM[0x801] = {};
uint8_t* GamePak = nullptr;

BIOS systemROM;
ExternalWorkRAM ewram;
InternalWorkRAM iwram;
PaletteRAM paletteram;
VRAM vram;
OAMRAM oamRam;
Sram sram;

std::array<unsigned char*, 16> memoryLayout = { []() constexpr {
	std::array<unsigned char*, 16> retArray { nullptr, nullptr, nullptr, nullptr, IoRAM, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	retArray[ESystemROM_L] = systemROM.getMemoryPtr();
	retArray[ESystemROM_H] = systemROM.getMemoryPtr();
	retArray[EExternalWorkRAM] = ewram.getMemoryPtr();
	retArray[EInternalWorkRAM] = iwram.getMemoryPtr();
	retArray[EPaletteRAM] = paletteram.getMemoryPtr();
	retArray[EVRAM] = vram.getMemoryPtr();
	retArray[EOAM] = oamRam.getMemoryPtr();
	retArray[ESRAM_L] = sram.getMemoryPtr();
	retArray[ESRAM_H] = sram.getMemoryPtr();

	return retArray;
}() };

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

void DmaIncreasing(uint32_t dmaNumber, uint32_t destination, uint32_t source, uint32_t size) {
	destination &= ~3;
	source &= ~3;

	uint32_t mask1 = (destination >> 24) & 15;
	uint32_t mask2 = (source >> 24) & 15;

	if (mask1 == EPaletteRAM) {
		paletteram.m_accessed = 1;
		paletteram.m_accessedPaletteColour.set();
		debugView->VRAMupdated = true;
		debugView->OBJupdated = true;
	}
		
	if (mask1 == EVRAM || mask1 == EOAM)
	{
		debugView->VRAMupdated = true;
		debugView->OBJupdated = true;
	}

	if (mask1 >= EGamePak1 && mask1 <= EGamePak6)
		return;
	if (mask1 == ESRAM_L || mask1 == ESRAM_H || mask2 == ESRAM_L || mask2 == ESRAM_H)
		return;
	if (mask1 == ESystemROM_L || mask1 == ESystemROM_H)
		return;

	destination = clampAddress(mask1, destination);
	source = clampAddress(mask2, source);

	memcpy(&memoryLayout[mask1][destination], &memoryLayout[mask2][source], size * 4);
}

void writeToAddress(uint32_t address, uint8_t value){
	MemoryAddress memDecoder{ address };
	calculateCycles(memDecoder.address, (previousAddress + 1) == memDecoder.address);

	if (memDecoder.mask == ESystemROM_L || memDecoder.mask == ESystemROM_H) {
		systemROM.write8(memDecoder, value);
		return;
	}

	if (memDecoder.mask == 0x2){
		ewram.write8(memDecoder, value);
		return;
	}

	if (memDecoder.mask == 0x3) {
		iwram.write8(memDecoder, value);
		return;
	}

	if (memDecoder.mask == 0x5) {
		paletteram.write8(memDecoder, value);
		return;
	}

	if (memDecoder.mask == 0x6) {
		vram.write8(memDecoder, value);
		debugView->VRAMupdated = true;
		debugView->OBJupdated = true;
		return;
	}

	if (memDecoder.mask == EOAM) {
		oamRam.write8(memDecoder, value);
		return;
	}

	if (memDecoder.mask == 0xe || memDecoder.mask == 0xf){
		sram.write8(memDecoder, value);
		return;
	}

	if (memDecoder.mask == 4 && memDecoder.address > memsizes[memDecoder.mask])
		return;

	else if ((memDecoder.mask == 5 || memDecoder.mask == 6 || memDecoder.mask == 7) && (displayCtrl->forceBlank || InterruptFlagRegister->vBlank || InterruptFlagRegister->hBlank))
		return;

	else if (memDecoder.mask == 0)
		return;

	else if (memDecoder.mask >= 8 && memDecoder.mask <= 0xd)
		return;
	address = clampAddress(memDecoder.mask, memDecoder.address);

	if (memDecoder.mask == 7
		|| (displayCtrl->bgMode == 7 && memDecoder.mask == 6 && address >= 0x10000)
		|| (displayCtrl->bgMode == 4 && memDecoder.mask == 6 && address >= 0x10000))
		return;

	if ((displayCtrl->bgMode == 4 && memDecoder.mask == 6 && address < 0x10000)
		|| memDecoder.mask == 5){
		memoryLayout[memDecoder.mask][address] = value;
		memoryLayout[memDecoder.mask][address + 1] = value;
		return;
	}

	if (specialWrites(memDecoder.mask, address, value))
		return;

	memoryLayout[memDecoder.mask][address] = value;
}

void writeToAddress16(uint32_t address, uint16_t value){
	calculateCycles(address, (previousAddress + 2) == address);
	MemoryAddress memDecoder{ address };
	uint32_t mask = memDecoder.mask;
	address &= ~0xFF000000;
	uint32_t misalignment = address & 1;

	if (memDecoder.mask == ESystemROM_L || memDecoder.mask == ESystemROM_H) {
		systemROM.write16(memDecoder, value);
		return;
	}

	if (memDecoder.mask == 0x2){
		ewram.write16(memDecoder, value);
		return;
	}

	if (memDecoder.mask == 0x3) {
		iwram.write16(memDecoder, value);
		return;
	}

	if (memDecoder.mask == 0x5) {
		paletteram.write16(memDecoder, value);
		return;
	}

	if (memDecoder.mask == 0x6) {
		vram.write16(memDecoder, value);
		debugView->VRAMupdated = true;
		debugView->OBJupdated = true;
		return;
	}

	if (memDecoder.mask == EOAM) {
		oamRam.write16(memDecoder, value);
		return;
	}

	else if (memDecoder.mask >= 8 && memDecoder.mask <= 0xd)
		return;
	
	if (mask == 0xe || mask == 0xf)
	{
		sram.write16(memDecoder, value);
		return;
	}

	if (mask == 4 && address > memsizes[mask])
		return;
	else if (mask == 0)
		return;
	else if (mask >= 8 && mask <= 0xd)
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
	MemoryAddress memDecoder{ address };
	uint32_t mask = memDecoder.mask;
	address &= ~0xFF000000;
	uint32_t misalignment = address & 3;

	if (memDecoder.mask == ESystemROM_L || memDecoder.mask == ESystemROM_H) {
		systemROM.write32(memDecoder, value);
		return;
	}

	if (memDecoder.mask == 0x2) {
		ewram.write32(memDecoder, value);
		return;
	}

	if (memDecoder.mask == 0x3) {
		iwram.write32(memDecoder, value);
		return;
	}

	if (memDecoder.mask == 0x5) {
		paletteram.write32(memDecoder, value);
		return;
	}

	if (memDecoder.mask == 0x6) {
		vram.write32(memDecoder, value);
		debugView->OBJupdated = true;
		debugView->VRAMupdated = true;
		return;
	}

	if (memDecoder.mask == EOAM) {
		oamRam.write32(memDecoder, value);
		return;
	}

	else if (memDecoder.mask >= 8 && memDecoder.mask <= 0xd)
		return;

	if (mask == 0xe || mask == 0xf)
	{
		sram.write32(memDecoder, value);
		return;
	}

	if (mask == 4 && address > memsizes[mask])
		return;

	else if (mask == 0)
		return;

	else if (mask >= 8 && mask <= 0xd)
		return;
	

	address = clampAddress(mask, address);

	if (specialWrites(mask, address, value))
		return;

	*(uint32_t*)&(uint8_t)memoryLayout[mask][address - misalignment] = value;
}

uint8_t loadFromAddress(uint32_t address, bool free){
	if (!free)
		calculateCycles(address, (previousAddress + 1) == address);

	MemoryAddress memDecoder{ address };
	uint32_t mask = memDecoder.mask;

	if (memDecoder.mask == ESystemROM_L || memDecoder.mask == ESystemROM_H) {
		return systemROM.read8(r, memDecoder);
	}

	if (memDecoder.mask == 0x2){
		return ewram.read8(memDecoder);
	}

	if (memDecoder.mask == 0x3) {
		return iwram.read8(memDecoder);
	}

	if (memDecoder.mask == 0x5) {
		return paletteram.read8(memDecoder);
	}

	if (memDecoder.mask == 0x6) {
		return vram.read8(memDecoder);
	}

	if (memDecoder.mask == EOAM) {
		return oamRam.read8(memDecoder);
	}

	if (mask == 0xe || mask == 0xf){
		return sram.read8(memDecoder);
	}

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

	MemoryAddress memDecoder{ address };
	uint32_t mask = memDecoder.mask;
	address &= ~0xFF000000;
	
	if (memDecoder.mask == ESystemROM_L || memDecoder.mask == ESystemROM_H) {
		return systemROM.read16(r, memDecoder);
	}

	if (memDecoder.mask == 0x2){
		return ewram.read16(memDecoder);
	}
	if (memDecoder.mask == 0x3) {
		return iwram.read16(memDecoder);
	}
	if (memDecoder.mask == 0x5) {
		return paletteram.read16(memDecoder);
	}
	if (memDecoder.mask == 0x6) {
		return vram.read16(memDecoder);
	}

	if (memDecoder.mask == EOAM) {
		return oamRam.read16(memDecoder);
	}

	if (mask == 0xe || mask == 0xf){
		return sram.read16(memDecoder);
	}

	if (mask == 4 && address > memsizes[mask])
		return 0;

	address = clampAddress(mask, address);
	uint16_t tmpResult = rawLoad16(memoryLayout[mask], (address - misalignment));

	auto tmp = RORnoCond(tmpResult, 8 * misalignment);
	return tmp;
}

uint32_t loadFromAddress32(uint32_t address, bool free){
	if (!free){
		calculateCycles(address, (previousAddress + 4) == address);
		calculateCycles(address, true);
		cycles += 1;
	}

	uint32_t misalignment = address & 3;

	MemoryAddress memDecoder{ address };
	uint32_t mask = memDecoder.mask;

	if (memDecoder.mask == ESystemROM_L || memDecoder.mask == ESystemROM_H) {
		return systemROM.read32(r, memDecoder);
	}

	if (memDecoder.mask == 0x2){
		return ewram.read32(r, memDecoder);
	}

	if (memDecoder.mask == 0x3) {
		return iwram.read32(r, memDecoder);
	}

	if (memDecoder.mask == 0x5) {
		return paletteram.read32(r, memDecoder);
	}
	if (memDecoder.mask == 0x6) {
		return vram.read32(r, memDecoder);
	}

	if (memDecoder.mask == EOAM) {
		return oamRam.read32(r, memDecoder);
	}
	
	if (mask == 0xe || mask == 0xf){
		return sram.read32(r, memDecoder);
	}

	address &= ~0xFF000000;

	if (mask == 4 && address > memsizes[mask])
		return 0;

	address = clampAddress(mask, address);

	uint32_t result = 0;

	if (r[15] < 0x4000)
		result = rawLoad32(memoryLayout[mask], address - misalignment);
	else
		result = RORnoCond(rawLoad32(memoryLayout[mask], address - misalignment), (8 * misalignment));

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
	memoryLayout[EGamePak1] = GamePak;
	memoryLayout[EGamePak2] = &GamePak[0x1000000];
	memoryLayout[EGamePak3] = GamePak;
	memoryLayout[EGamePak4] = &GamePak[0x1000000];
	memoryLayout[EGamePak5] = GamePak;
	memoryLayout[EGamePak6] = &GamePak[0x1000000];
}
