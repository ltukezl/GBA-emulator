#ifndef MEMORYOPS_H
#define MEMORYOPS_H

#include <cstdint>
#include <array>
#include "Memory/MemoryAreas/BIOS.h"
#include "Memory/MemoryAreas/EWRAM.h"
#include "Memory/MemoryAreas/IWRAM.h"
#include "Memory/MemoryAreas/PaletteRAM.h"
#include "Memory/MemoryAreas/VRAM.h"
#include "Memory/MemoryAreas/OAMRAM.h"
#include "Memory/MemoryAreas/SRAM.h"

extern BIOS systemROM;
extern ExternalWorkRAM ewram;
extern InternalWorkRAM iwram;
extern uint8_t IoRAM[0x801];
extern PaletteRAM paletteram;
extern VRAM vram;
extern OAMRAM oamRam;
extern Sram sram;
extern uint8_t* GamePak;
extern std::array<unsigned char*, 16> memoryLayout;

// create memory for the different memory areas

void DmaIncreasing(uint32_t dmaNumber, MemoryAddress destination, MemoryAddress source, uint32_t size);

void rawWrite8(uint8_t* arr, uint32_t addr, uint8_t val);
void rawWrite16(uint8_t* arr, uint32_t addr, uint16_t val);
void rawWrite32(uint8_t* arr, uint32_t addr, uint32_t val);

uint8_t rawLoad8(uint8_t* arr, uint32_t addr);
uint16_t rawLoad16(uint8_t* arr, uint32_t addr);
uint32_t rawLoad32(uint8_t* arr, uint32_t addr);

uint8_t loadFromAddress(uint32_t addr, bool free = false);
uint32_t loadFromAddress16(uint32_t addr, bool free = false);
uint32_t loadFromAddress32(uint32_t addr, bool free = false);

void writeToAddress(uint32_t addr, uint8_t val);
void writeToAddress16(uint32_t addr, uint16_t val);
void writeToAddress32(uint32_t addr, uint32_t val);

void PUSH(int);
unsigned __int32 POP();

void memoryInits();

#endif