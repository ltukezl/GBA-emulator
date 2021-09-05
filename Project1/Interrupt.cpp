#include "Constants.h"
#include "MemoryOps.h"
#include "GBAcpu.h"
#include "memoryMappedIO.h"
#include <iostream>

int vBlankCounter = 0;
int hBlankCounter = 0;
bool IRQMode = false;

void interruptController(){
	debug = false;
	InterruptMaster.addr = loadFromAddress16(0x4000208, true);
	if (!InterruptMaster.IRQEnabled || cpsr.IRQDisable){
		return;
	}

	r = svc;
	*r[14] = *r[PC] + 4;
	*r[16] = cpsr.val;
	//svc mode
	cpsr.IRQDisable = 1;
	cpsr.thumb = 0;
	cpsr.mode = SUPER;

	*r[PC] = 0x8;
}

void HWInterrupts(int cycles){
	InterruptMaster.addr = loadFromAddress16(0x4000208, true) & 1;

	if (!InterruptMaster.IRQEnabled || cpsr.IRQDisable){
		return;
	}
	
	if (InterruptEnableRegister.hBlank && LCDstatus.hIRQEn){
		if (hBlankCounter > (hBlankCounter + cycles) % 1232){
			InterruptFlagRegister.addr = rawLoad16(IoRAM, 0x202);
			InterruptFlagRegister.hBlank = 1;
			rawWrite16(IoRAM, 0x202, InterruptFlagRegister.addr);
			LCDstatus.hblankFlag = 1;
		}
		hBlankCounter = (hBlankCounter + cycles) % 1232;
	}

	if (InterruptEnableRegister.vBlank && LCDstatus.vIRQEn){
		if (vBlankCounter > (vBlankCounter + cycles) % 280896){
			InterruptFlagRegister.addr = rawLoad16(IoRAM, 0x202);
			InterruptFlagRegister.vBlank = 1;
			rawWrite16(IoRAM, 0x202, InterruptFlagRegister.addr);
			LCDstatus.vblankFlag = 1;
		}
		vBlankCounter = (vBlankCounter + cycles) % 280896;
	}
	
	//InterruptFlagRegister.addr = loadFromAddress16(0x4000202, true);
	if (InterruptFlagRegister.addr != 0){
		//debug = true
		if (debug)
			std::cout << "entered interuut from 0x" << std::hex << *r[PC] << std::dec << std::endl;
		
		r = irq;
		*r[16] = cpsr.val;
		cpsr.thumb = 0;
		cpsr.IRQDisable = 1;
		
		*r[LR] = *r[PC] + 4;
		*r[PC] = 0x18;

		cpsr.mode = IRQ;
		
	}
}