#ifndef GBACPU_H
#define GBACPU_H
#include <stdint.h>
#include "cplusplusRewrite/HwRegisters.h"

extern __int32* usrSys[17];
extern __int32* abt[17];
extern __int32* undef[17];
extern __int32* fiq[17];
extern __int32* irq[17];
extern __int32* svc[17];

extern Registers r;

enum CpuModes{
	USR = 0x10,
	FIQ = 0x11,
	IRQ = 0x12,
	SUPER = 0x13,
	ABORT = 0x17,
	UNDEF = 0x1B,
	SYS = 0x1F
};

extern union CPSR {
	struct {
		uint32_t mode : 5;
		uint32_t thumb : 1;
		uint32_t FIQDisable : 1;
		uint32_t IRQDisable : 1;
		uint32_t unused : 20;
		uint32_t overflow : 1;
		uint32_t carry : 1;
		uint32_t zero : 1;
		uint32_t negative : 1;
	};
	uint32_t val;
}cpsr;

extern __int64 cycles;
extern __int8 N_cycles;
extern __int8 S_cycles;

extern uint8_t firstAccessCycles[4];
extern uint8_t WS0Second[2];
extern uint8_t WS1Second[2];
extern uint8_t WS2Second[2];

extern bool debug;
extern bool step;
extern bool memStatistics;

template<int start>
int signExtend(int value){
	int m = 1U << (start - 1); //bitextend hack
	int r = (value ^ m) - m;
	return r;
}

#endif // MAIN_H

