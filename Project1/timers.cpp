#include "timers.h"
#include "memoryMappedIO.h"
#include "MemoryOps.h"
#include <stdint.h>

enum prescaler {
	ONE_TO_ONE,
	ONE_TO_64,
	ONE_TO_256,
	ONE_TO_1024,
};

uint16_t reloads[4] = { 0 };
uint16_t called[4] = { 0 };

bool timerReloadWrite(uint32_t addr, uint32_t val){
	union TIMERCNT oldReg;
	union TIMERCNT newReg;
	oldReg.addr = loadFromAddress32(addr, true);
	newReg.addr = val;

	if (!oldReg.startStop && newReg.startStop){
		if (addr == 0x4000102){
			writeToAddress16(addr - 2, reloads[0]);
		}
		else if (addr == 0x4000106){
			writeToAddress16(addr - 2, reloads[1]);
		}
		else if (addr == 0x400010A){
			writeToAddress16(addr - 2, reloads[2]);
		}
		else if (addr == 0x400010E){
			writeToAddress16(addr - 2, reloads[3]);
		}
		return false;
	}

	if (addr == 0x4000100){
		reloads[0] = val;
	}
	else if (addr == 0x4000104){
		reloads[1] = val;
	}
	else if (addr == 0x4000108){
		reloads[2] = val;
	}
	else if (addr == 0x400010C){
		reloads[3] = val;
	}

	return true;
}

void updateTimers() {

	for (int i = 0; i < 4; i++){
		TIMERCNT.addr = loadFromAddress32(0x4000100 + 4 * i, true);
		if (TIMERCNT.startStop){
			if (TIMERCNT.cntrSelect == ONE_TO_ONE){
				TIMERCNT.counterVal += 1;
			}
			else if (TIMERCNT.cntrSelect == ONE_TO_64){
				if (called[i] >(called[i] + 1) % 64){
					TIMERCNT.counterVal += 1;
				}
				called[i] = (called[i] + 1) % 64;
			}
			else if (TIMERCNT.cntrSelect == ONE_TO_256){
				if (called[i] > (called[i] + 1) % 256){
					TIMERCNT.counterVal += 1;
				}
				called[i] = (called[i] + 1) % 256;
			}
			else if (TIMERCNT.cntrSelect == ONE_TO_1024){
				if (called[i] > (called[i] + 1) % 1024){
					TIMERCNT.counterVal += 1;
				}
				called[i] = (called[i] + 1) % 1024;
			}

			if (TIMERCNT.counterVal <= reloads[i]){
				if (i == 0 && InterruptEnableRegister.timer0OVF)
					InterruptFlagRegister.timer0OVF = 1;
				else if (i == 1 && InterruptEnableRegister.timer1OVF)
					InterruptFlagRegister.timer1OVF = 1;
				else if (i == 2 && InterruptEnableRegister.timer2OVF)
					InterruptFlagRegister.timer2OVF = 1;
				else if (i == 3 && InterruptEnableRegister.timer3OVF)
					InterruptFlagRegister.timer3OVF = 1;

				TIMERCNT.counterVal = reloads[i];

				if (i < 4 && TIMERCNT.timing && TIMERCNT.startStop){
					uint16_t oldVal = loadFromAddress16(0x4000100 + 4 * (i + 1));
					*(unsigned short*)&(unsigned char)memoryLayout[4][0x100 + 4 * (i + 1)] = oldVal + 1;
				}
			}
			*(unsigned short*)&(unsigned char)memoryLayout[4][0x100 + 4 * i] = TIMERCNT.counterVal;
		}
	}
}