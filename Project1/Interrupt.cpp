#include "Constants.h"
#include "MemoryOps.h"
#include "GBAcpu.h"


void interruptController(int opcode){
	int interuptNum = opcode & 0xFF;
	__int32 intEnabled = loadFromAddress32(0x4000208) & 1;
	__int32 irqDisable = (cprs >> 7) & 1;
	if (!intEnabled || irqDisable){
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
