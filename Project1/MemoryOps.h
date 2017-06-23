#ifndef MEMORYOPS_H
#define MEMORYOPS_H

extern unsigned char systemROM[0x3FFF];
extern unsigned char unused[0x3FFF];
extern unsigned char ExternalWorkRAM[0xFFFF];
extern unsigned char InternalWorkRAM[0x7FFF];
extern unsigned char IoRAM[0x3FF];
extern unsigned char PaletteRAM[0x3FF];
extern unsigned char VRAM[0x17FFF];
extern unsigned char OAM[0x3FF];
extern unsigned char GamePak[0xFFFFFF];

extern unsigned char* memoryLayout[15];

int loadFromAddress(int);
unsigned __int16 loadFromAddress16(int);
unsigned __int32 loadFromAddress32(int);

void writeToAddress(int, int);
void writeToAddress16(int, int);
void writeToAddress32(int, int);

void PUSH(int);
unsigned __int32 POP();

#endif