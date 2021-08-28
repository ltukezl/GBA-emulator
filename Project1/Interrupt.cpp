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
	if (!InterruptMaster.IRQEnabled || cpsr.IRQDisable){
		return;
	}

	r = svc;
	cpsr.val = 0;

	*r[14] = *r[PC];
	//svc mode
	cpsr.thumb = 0;
	cpsr.mode = SUPER;

	*r[PC] = 0x8;
}

void HWInterrupts(int cycles){
	__int32 intEnabled = loadFromAddress16(0x4000208, true) & 1;
	uint32_t currAddress = loadFromAddress32(*r[PC], true);

	if (!intEnabled || cpsr.IRQDisable){
		return;
	}
	
	if (InterruptEnableRegister.vBlank){
		if (vBlankCounter > (vBlankCounter + cycles) % 280896){
			InterruptFlagRegister.vBlank = 1;
		}
		vBlankCounter = (vBlankCounter + cycles) % 280896;
	}
	

	if (InterruptFlagRegister.addr != 0){
		std::cout << "entered interuut from 0x" << std::hex << *r[PC] << std::dec << std::endl;
		cpsr.thumb = 0;
		cpsr.IRQDisable = 1;
		cpsr.mode = IRQ;
		r = irq;
		*r[LR] = *r[PC] + 4;
		*r[PC] = 0x18;
		debug = true;
	}
}