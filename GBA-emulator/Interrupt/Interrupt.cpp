#include "Constants.h"
#include "Memory/MemoryOps.h"
#include "GBAcpu.h"
#include "Memory/memoryMappedIO.h"
#include <iostream>

#define ENABLED 1
bool IRQMode = false;

void interruptController(){
	//DURING IRQ
#if ENABLED
	//debug = true;
	r.updateMode(CpuModes_t::ESUPER);
	r[LR] = r[TRegisters::EProgramCounter];
	r[16] = cpsr.val;
	//svc mode
	cpsr.IRQDisable = 1;
	cpsr.thumb = 0;
	cpsr.mode = SUPER;

	r[TRegisters::EProgramCounter] = 0x8;
#endif
}

bool timerCountHappened[4] = {};
uint64_t timerCount[4] = {};

void HWInterrupts(uint64_t cycles){
	//DURING IRQ
#if ENABLED
	if (!InterruptMaster->IRQEnabled || cpsr.IRQDisable){
		return;
	}
		
	uint16_t mask = 1 << 3;
	for (int timerInt = 0; timerInt < 4; timerInt++){
		if (InterruptFlagRegister->addr & mask){
			InterruptFlagRegister->addr &= ~mask;
			if (!timerCountHappened[timerInt])
				timerCount[timerInt] = 5;
			timerCountHappened[timerInt] = true;
		}

		if (timerCountHappened[timerInt] && timerCount[timerInt] > 0){
			timerCount[timerInt] -= cycles;
		}
		else if (timerCountHappened[timerInt] && timerCount[timerInt] <= 0){
			InterruptFlagRegister->addr |= mask;
			timerCountHappened[timerInt] = false;
		}
		mask <<= 1;
	}

	if (InterruptFlagRegister->addr != 0){
		if (debug)
			std::cout << "entered interrupt from 0x" << std::hex << r[TRegisters::EProgramCounter] << " saving LR " << (cpsr.thumb ? r[PC] + 4 : r[PC]) << " " << std::dec << std::endl;
		//debug = true;
		r.updateMode(CpuModes_t::EIRQ);
		r[16] = cpsr.val;

		r[LR] = r[TRegisters::EProgramCounter] + 4;
		r[TRegisters::EProgramCounter] = 0x18;
		cpsr.thumb = 0;
		cpsr.IRQDisable = 1;
		cpsr.mode = IRQ;
	}
#endif
}