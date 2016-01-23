#include "Constants.h"
#include "MemoryOps.h"
#include "GBAcpu.h"
#include "iostream"

uint32_t systemROMStart = 0x00000000;
uint32_t ExternalWorkRAMStart = 0x02000000;
uint32_t InternalWorkRAMStart = 0x03000000;
uint32_t IoRAMStart = 0x04000000;
uint32_t PaletteRAMStart = 0x05000000;
uint32_t VRAMStart = 0x06000000;
uint32_t OAMStart = 0x07000000;
uint32_t StackStart = 0x03007F00;

char systemROM[0x3FFF];
char unused[0x3FFF];
char ExternalWorkRAM[0xFFFF];
char InternalWorkRAM[0x7FFF];
char IoRAM[0x3FF];
char PaletteRAM[0x3FF];
char VRAM[0x17FFF];
char OAM[0x3FF];
//pop	{r7, pc}
char *memoryLayout[8] = { systemROM, unused, ExternalWorkRAM, InternalWorkRAM, IoRAM, PaletteRAM, VRAM, OAM };

void writeToAddress(int address, int value){
    int mask = (address >> 24) & 7;
	memoryLayout[mask][address - (mask << 24)] = value;
}

int loadFromAddress(int address){
    int mask = (address >> 24) & 7;
	return memoryLayout[mask][address - (mask << 24)];
}

void writeToAddress32(int address, int value){
    int mask = (address >> 24) & 7;

	memoryLayout[mask][address - (mask << 24) + 3] = value & 0xFF;
	memoryLayout[mask][address - (mask << 24) + 2] = (value >> 8) & 0xFF;
	memoryLayout[mask][address - (mask << 24) + 1] = (value >> 16) & 0xFF;
	memoryLayout[mask][address - (mask << 24) + 0] = (value >> 24) & 0xFF;
}

__int32 loadFromAddress32(int address){
    int mask = (address >> 24) & 7;
	int number = 0;

	number |= (unsigned char)memoryLayout[mask][address - (mask << 24) + 0] << 24;
	number |= (unsigned char)memoryLayout[mask][address - (mask << 24) + 1] << 16;
	number |= (unsigned char)memoryLayout[mask][address - (mask << 24) + 2] << 8;
	number |= (unsigned char)memoryLayout[mask][address - (mask << 24) + 3];
	return number;
}

void writeToAddress16(int address, int value){
    int mask = (address >> 24) & 7;

	memoryLayout[mask][address - (mask << 24) + 1] = value & 0xFF;
	memoryLayout[mask][address - (mask << 24) + 0] = (value >> 8) & 0xFF;
}

__int16 loadFromAddress16(int address){
    int mask = (address >> 24) & 7;
	int number = 0;

	number |= (unsigned char)memoryLayout[mask][address - (mask << 24)] << 8;
	number |= (unsigned char)memoryLayout[mask][address - (mask << 24) + 1];
	return number;
}

void PUSH(int value){
    SP -= 4;
    std::cout << "Pushed " << value <<" to "<< SP <<"\n";
	writeToAddress32(SP, value);
}

int POP(){

    int value = loadFromAddress32(SP);
    std::cout << "Popped " << value <<" from "<< SP <<" to ";
    SP += 4;
    return value;
}
