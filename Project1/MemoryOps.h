#ifndef MEMORYOPS_H
#define MEMORYOPS_H

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

int loadFromAddress(int);
unsigned __int16 loadFromAddress16(int);
unsigned __int32 loadFromAddress32(int);

void writeToAddress(int, int);
void writeToAddress16(int, int);
void writeToAddress32(int, int);

void PUSH(int);
unsigned __int32 POP();

#endif