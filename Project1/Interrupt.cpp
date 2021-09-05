#include "Constants.h"
#include "MemoryOps.h"
#include "GBAcpu.h"
#include "memoryMappedIO.h"
#include <iostream>
#include <stdint.h>

int vBlankCounter = 0;
int hBlankCounter = 0;
bool IRQMode = false;

class SWI {
	union CPSR* m_cpsr;
	uint16_t m_IME = 0;

protected:
	SWI(uint16_t IMEAddr, union CPSR* cpsr){
		m_IME = IMEAddr;
		m_cpsr = cpsr;
	}

	bool enabled(){
		InterruptMaster.addr = rawLoad16(IoRAM, m_IME);
		if (!InterruptMaster.IRQEnabled || m_cpsr->IRQDisable){
			return false;
		}
		return true;
	}

	void execute(){
		saveContext();
		updateStatusReg();
		*r[PC] = 0x8;
	}

	void saveContext(){
		*r[14] = *r[PC];
		*r[16] = m_cpsr->val;
	}

	void updateStatusReg(){
		m_cpsr->IRQDisable = 1;
		m_cpsr->thumb = 0;
		m_cpsr->mode = SUPER;
	}
};

class SupervisorMode : SWI{
public:
	SupervisorMode(uint16_t IMEAddr, union CPSR* cpsr) : SWI(IMEAddr, cpsr){

	}
	void sendInterrupt(){
		r = svc;
		execute();
	}
};

SupervisorMode* swi = new SupervisorMode(0x208, &cpsr);

void interruptController(){
	

}

void HWInterrupts(int cycles){
	InterruptMaster.addr = rawLoad16(IoRAM, 0x208);

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