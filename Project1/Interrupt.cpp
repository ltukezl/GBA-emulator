#include "Constants.h"
#include "MemoryOps.h"
#include "GBAcpu.h"
#include "memoryMappedIO.h"


void interruptController(int opcode){
	int interuptNum = opcode & 0xFF;

	InterruptMaster.addr = loadFromAddress16(0x4000208, true);
	__int32 irqDisable = (cprs >> 7) & 1;
	if (!InterruptMaster.IRQEnabled || irqDisable){
		return;
	}

	r = svc;
	*r[16] = cprs;

	*r[14] = *r[PC];
	//svc mode
	cprs &= ~(1 << 5);
	cprs &= ~0x1f;
	cprs |= 0x93;

	*r[PC] = 0x8;
}

void HWInterrupts(int cycles){
	__int32 intEnabled = loadFromAddress16(0x4000208, true) & 1;
	__int32 irqDisable = (cprs >> 7) & 1;
	

	if (cycles > 280896){//V-blank	
		cycles -= 280896;

		if (!intEnabled || irqDisable){
			return;
		}

		if (!InterruptEnableRegister.vBlank)
			return;

		InterruptFlagRegister.vBlank = 1;

		r = irq;
		*r[PC] = 0x18;
	}
}