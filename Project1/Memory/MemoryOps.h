#ifndef MEMORYOPS_H
#define MEMORYOPS_H

#include <stdint.h>

extern uint8_t systemROM[0x4000];
extern uint8_t IoRAM[0x801];
extern uint8_t PaletteRAM[0x400];
extern uint8_t VRAM[0x18000];
extern uint8_t OAM[0x400];
extern uint8_t* GamePak;
extern unsigned char* memoryLayout[16];

void DmaIncreasing(uint32_t destination, uint32_t source, uint32_t size);

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