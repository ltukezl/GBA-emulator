#include "timers.h"
#include "memoryMappedIO.h"
#include "MemoryOps.h"
#include "GBAcpu.h"
#include <stdint.h>
#include <iostream>

enum prescaler {
	ONE_TO_ONE,
	ONE_TO_64,
	ONE_TO_256,
	ONE_TO_1024,
};

uint16_t reloads[4] = { 0 };
uint16_t called[4] = { 0 };

bool timerReloadWrite(uint32_t addr, uint32_t val){
	union TIMERCNT newReg;
	newReg.addr = val;

	//debug = true;

	

	TIMERCNT* timerCtrl = (TIMERCNT*)&IoRAM[addr];

	if (addr == 0x100){
		reloads[0] = val;
	}
	else if (addr == 0x104){
		reloads[1] = val;
	}
	else if (addr == 0x108){
		reloads[2] = val;
	}
	else if (addr == 0x10B){
		reloads[3] = val;
	}

	if (!timerCtrl->startStop && newReg.startStop){
		rawWrite32(IoRAM, addr, val);
	}

	return true;
}

bool reloadCounter(uint32_t addr, uint32_t val){
	union TIMERCNT oldReg;
	union TIMERCNT newReg;
	oldReg.addr = rawLoad32(IoRAM, addr - 2);
	newReg.addr = val;

	if (!oldReg.startStop && newReg.startStop){
		if (addr == 0x102){
			rawWrite16(IoRAM, addr - 2, reloads[0]);
		}
		else if (addr == 0x106){
			rawWrite16(IoRAM, addr - 2, reloads[1]);
		}
		else if (addr == 0x10A){
			rawWrite16(IoRAM, addr - 2, reloads[2]);
		}
		else if (addr == 0x10D){
			rawWrite16(IoRAM, addr - 2, reloads[3]);
		}
	}

	return false;
}

void updateTimers(uint32_t cycles) {

	for (int i = 0; i < 4; i++){
		TIMERCNT* timerCtrl = (TIMERCNT*)&IoRAM[0x100 + 4 * i];
		if (timerCtrl->startStop){
			if (timerCtrl->cntrSelect == ONE_TO_ONE){
				timerCtrl->counterVal += cycles;
			}
			else if (timerCtrl->cntrSelect == ONE_TO_64){
				if (called[i] >(called[i] + cycles) % 64){
					timerCtrl->counterVal += 1;
				}
				called[i] = (called[i] + cycles) % 64;
			}
			else if (timerCtrl->cntrSelect == ONE_TO_256){
				if (called[i] > (called[i] + cycles) % 256){
					timerCtrl->counterVal += 1;
				}
				called[i] = (called[i] + cycles) % 256;
			}
			else if (timerCtrl->cntrSelect == ONE_TO_1024){
				if (called[i] > (called[i] + cycles) % 1024){
					timerCtrl->counterVal += 1;
				}
				called[i] = (called[i] + cycles) % 1024;
			}

			if (timerCtrl->counterVal == 0){
				if (i == 0 && InterruptEnableRegister->timer0OVF)
					InterruptFlagRegister->timer0OVF = 1;
				else if (i == 1 && InterruptEnableRegister->timer1OVF)
					InterruptFlagRegister->timer1OVF = 1;
				else if (i == 2 && InterruptEnableRegister->timer2OVF)
					InterruptFlagRegister->timer2OVF = 1;
				else if (i == 3 && InterruptEnableRegister->timer3OVF)
					InterruptFlagRegister->timer3OVF = 1;

				if (i < 4 && timerCtrl->timing && timerCtrl->startStop){
					uint16_t oldVal = loadFromAddress16(0x4000100 + 4 * (i + 1));
					*(unsigned short*)&(unsigned char)memoryLayout[4][0x100 + 4 * (i + 1)] = oldVal + 1;
				}

				timerCtrl->counterVal = reloads[i];
			}
		}
	}
}