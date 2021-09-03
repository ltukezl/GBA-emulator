#ifndef MEMORYOPS_H
#define MEMORYOPS_H

#include <stdint.h>

extern unsigned char systemROM[0x4000];
extern unsigned char unused[0x4000];
extern unsigned char ExternalWorkRAM[0x1000000];
extern unsigned char InternalWorkRAM[0x1000000];
extern unsigned char IoRAM[0x1000000];
extern unsigned char PaletteRAM[0x1000000];
extern unsigned char VRAM[0x1000000];
extern unsigned char OAM[0x1000000];
extern unsigned char GamePak[0x2000000];
extern unsigned char GamePakSRAM[0x2000000];
extern unsigned char* memoryLayout[16];

void rawWrite8(uint8_t* arr, uint32_t addr, uint8_t val);
void rawWrite16(uint8_t* arr, uint32_t addr, uint16_t val);
void rawWrite32(uint8_t* arr, uint32_t addr, uint32_t val);

uint8_t rawLoad8(uint8_t* arr, uint32_t addr);
uint16_t rawLoad16(uint8_t* arr, uint32_t addr);
uint32_t rawLoad32(uint8_t* arr, uint32_t addr);

uint8_t loadFromAddress(uint32_t addr, bool free = false);
uint16_t loadFromAddress16(uint32_t addr, bool free = false);
uint32_t loadFromAddress32(uint32_t addr, bool free = false);

void writeToAddress(uint32_t addr, uint8_t val);
void writeToAddress16(uint32_t addr, uint16_t val);
void writeToAddress32(uint32_t addr, uint32_t val);

void PUSH(int);
unsigned __int32 POP();

void memoryInits();

#endif