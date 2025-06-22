#ifndef GBACPU_H
#define GBACPU_H
#include <cstdint>

#include "cplusplusRewrite/HwRegisters.h"

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

extern __int64 cycles;
extern __int8 N_cycles;
extern __int8 S_cycles;

extern uint8_t firstAccessCycles[4];
extern uint8_t WS0Second[2];
extern uint8_t WS1Second[2];
extern uint8_t WS2Second[2];

template<int start>
int signExtend(int value){
	int m = 1U << (start - 1); //bitextend hack
	int r = (value ^ m) - m;
	return r;
}

#endif // MAIN_H

