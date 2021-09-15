#include "Constants.h"
#include "MemoryOps.h"
#include "GBAcpu.h"
#include "memoryMappedIO.h"
#include <iostream>

#define ENABLED 1

int vBlankCounter = 0;
int hBlankCounter = 0;
bool IRQMode = false;

void interruptController(){
#if ENABLED
	//debug = true;
	r = svc;
	*r[14] = *r[PC];
	*r[16] = cpsr.val;
	//svc mode
	cpsr.IRQDisable = 1;
	cpsr.thumb = 0;
	cpsr.mode = SUPER;

	*r[PC] = 0x8;
#endif
}

bool timerCountHappened = false;
uint8_t timerCount = 0;

void HWInterrupts(int cycles){
#if ENABLED
	if (!InterruptMaster->IRQEnabled || cpsr.IRQDisable){
		return;
	}
	
	if (InterruptEnableRegister->hBlank){
		if (hBlankCounter > (hBlankCounter + cycles) % 1232){
			InterruptFlagRegister->hBlank = 1;
			LCDStatus->hblankFlag = 1;
		}
		hBlankCounter = (hBlankCounter + cycles) % 1232;
	}

	if (InterruptEnableRegister->vBlank){
		if (vBlankCounter > (vBlankCounter + cycles) % 280896){
			InterruptFlagRegister->vBlank = 1;
			LCDStatus->vblankFlag = 1;
		}
		vBlankCounter = (vBlankCounter + cycles) % 280896;
	}
	
	if (InterruptFlagRegister->timer0OVF){
		InterruptFlagRegister->timer0OVF = 0;
		if (!timerCountHappened)
			timerCount = 5;
		timerCountHappened = true;
	}

	if (timerCountHappened && timerCount != 0){
		timerCount--;
	}
	else if (timerCountHappened && timerCount == 0){
		InterruptFlagRegister->timer0OVF = 1;
		timerCountHappened = false;
		//debug = true;
	}


	if (InterruptFlagRegister->addr != 0){
		if (debug)
			std::cout << "entered interuut from 0x" << std::hex << *r[PC] << " saving LR " << (cpsr.thumb ? *r[PC] + 4 : *r[PC]) << " " << std::dec << std::endl;
		
		r = irq;
		*r[16] = cpsr.val;

		*r[LR] = *r[PC] + 4;
		*r[PC] = 0x18;

		cpsr.thumb = 0;
		cpsr.IRQDisable = 1;
		cpsr.mode = IRQ;
	}
#endif
}