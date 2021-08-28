#include "Constants.h"
#include "MemoryOps.h"
#include "GBAcpu.h"
#include "memoryMappedIO.h"
#include <iostream>

int vBlankCounter = 0;
bool IRQMode = false;

void interruptController(int opcode){
	int interuptNum = opcode & 0xFF;

	InterruptMaster.addr = loadFromAddress16(0x4000208, true);
	__int32 irqDisable = (cprs >> 7) & 1;
	if (!InterruptMaster.IRQEnabled || irqDisable){
		return;
	}

	r = svc;
	cprs = cprs;

	*r[14] = *r[PC];
	//svc mode
	cprs &= ~(1 << 5);
	cprs &= ~0x1f;
	cprs |= 0x93;

	*r[PC] = 0x8;
}

void HWInterrupts(int cycles){
	__int32 intEnabled = loadFromAddress16(0x4000208, true) & 1;
	__int32 irqDisable = cprs >> 7 & 1;
	uint32_t currAddress = loadFromAddress32(*r[PC], true);

	if (!intEnabled || irqDisable){
		return;
	}
	/*
	if (InterruptEnableRegister.vBlank){
		if (vBlankCounter > (vBlankCounter + cycles) % 280896){
			InterruptFlagRegister.vBlank = 1;
		}
		vBlankCounter = (vBlankCounter + cycles) % 280896;
	}
	*/
	if (InterruptFlagRegister.addr != 0){
		std::cout << "entered interuut from 0x" << std::hex << *r[PC] << std::dec << std::endl;
		cprs &= ~(1 << 5);
		cprs &= ~0xff;
		cprs |= 1 << 7;
		cprs |= 0x12;
		r = irq;
		*r[LR] = *r[PC] + 4;
		*r[PC] = 0x18;
	}
}