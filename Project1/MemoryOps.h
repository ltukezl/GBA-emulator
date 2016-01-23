#ifndef MEMORYOPS_H
#define MEMORYOPS_H

extern char systemROM[0x3FFF];
extern char unused[0x3FFF];
extern char ExternalWorkRAM[0xFFFF];
extern char InternalWorkRAM[0x7FFF];
extern char IoRAM[0x3FF];
extern char PaletteRAM[0x3FF];
extern char VRAM[0x17FFF];
extern char OAM[0x3FF];

int loadFromAddress(int);
__int16 loadFromAddress16(int);
__int32 loadFromAddress32(int);

void writeToAddress(int, int);
void writeToAddress16(int, int);
void writeToAddress32(int, int);

void PUSH(int);
int POP();

#endif