#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "Memory/MemoryOps.h"
#include "Thumb/ThumbOpCodes.h"
#include "Arm/ARMOpCodes.h"
#include "Constants.h"
#include "GBAcpu.h"
#include "DMA/DMA.h"
#include "Display/Display.h"
#include "Display/MemoryViewer.h"
#include "Display/PaletteViewer.h"
#include "Interrupt/interrupt.h"
#include "Timer/timers.h"
#include "Memory/memoryMappedIO.h"
#include "cplusplusRewrite/HwRegisters.h"
#include "cplusplusRewrite/dataProcessingOp.h"
#include "cplusplusRewrite/tests/allTests.h"

#define BIOS_START 0
#define MEMORY_VIEWER 0
#define PALETTE_VIEWER 0

using namespace std;

bool memStatistics = false;
bool debug = false;

__int64 vBlankCounter = 0;
__int64 hBlankCounter = 0;

bool hBlankHappened = false;
bool vBlankHappened = false;

Registers r;

union CPSR cpsr;
bool step = false;
//1  0  0  0
//N  Z  C  V  = sign,zero,carry,overflow

__int64 cycles = 0;
__int64 prevCycles = 0;
__int8 N_cycles = 0;
__int8 S_cycles = 0;
/*
uint8_t firstAccessCycles[4] = { 4, 3, 2, 8 };
uint8_t WS0Second[2] = { 2, 1 };
uint8_t WS1Second[2] = { 4, 1 };
uint8_t WS2Second[2] = { 8, 1 };
*/

uint8_t firstAccessCycles[4] = { 1, 1, 1, 1 };
uint8_t WS0Second[2] = { 1, 1 };
uint8_t WS1Second[2] = { 1, 1 };
uint8_t WS2Second[2] = { 1, 1 };

void updateInstructionCycleTimings(uint32_t address){
	if (address >= 0xC00'0000){
		N_cycles = firstAccessCycles[waitStateControl->waitstate2First];
		S_cycles = WS2Second[waitStateControl->waitstate2Second];
	}
	else if (address >= 0xA000000){
		N_cycles = firstAccessCycles[waitStateControl->waitstate1First];
		S_cycles = WS1Second[waitStateControl->waitstate1Second];
	}
	else if (address >= 0x8000000){
		N_cycles = firstAccessCycles[waitStateControl->waitstate0First];
		S_cycles = WS0Second[waitStateControl->waitstate0Second];
	}
}

Display* debugView;

/*
NOTE *r[PC] = 0x08000000 can be used to skip bios check but needs to start in usr mode.
otherwise gba starts from addrs 0 in svc mode
*/
int main(int argc, char *args[]){

	runAllTests();


#if BIOS_START
	cpsr.FIQDisable = 1;
	cpsr.IRQDisable = 1;
	cpsr.mode = SUPER;
#else
	cpsr.val = 0x1f;	//current program status register
#endif

	rawWrite16(IoRAM, 0x130, 0xFFFF); // input register, 0 = pressed down, 1 = released

	std::string windowName = "paletteWindow";
	debugView = new Display(1280, 496 * 2, windowName);
#if MEMORY_VIEWER
	MemoryViewer memoryViewer;
#endif
#if PALETTE_VIEWER
	PaletteViewer paletteViewer;
#endif
	//Display gameDisplay(240, 160, "game");

#if BIOS_START
	r.updateMode(CpuModes_t::ESYS);
	r[13] = SP_usr;
	r[16] = 0x10;
#else
	r.updateMode(CpuModes_t::ESUPER);
	r[TRegisters::EStackPointer] = 0x3007FE0;
	r[16] = 0x10;
#endif
	r.updateMode(CpuModes_t::EIRQ);
	r[TRegisters::EStackPointer] = SP_irq;
	r[16] = 0x10;

#if BIOS_START
	r.updateMode(CpuModes_t::ESUPER);
	r[13] = SP_svc;
	cpsr.mode = SUPER;
#else
	r.updateMode(CpuModes_t::EUSR);
	r[13] = SP_usr;
#endif

#if BIOS_START
	r[PC] = 0;
#else
	r[TRegisters::EProgramCounter] = 0x0800'0000;
#endif


#if BIOS_START

#else
	memoryLayout[4][6] = 0x9A; //Vcount initializtion use 7e when cycle counting is ready
#endif
	memoryInits();

	FILE *file;
	FILE *bios;
	fopen_s(&file, "GBA-emulator/TestBinaries/program3.bin", "rb");
	//fopen_s(&file, "GBA-emulator/TestBinaries/tonc/bigmap.gba", "rb");
	//fopen_s(&file, "GBA-emulator/TestBinaries/tonc/obj_demo.gba", "rb");
	//fopen_s(&file, "GBA-emulator/TestBinaries/tonc/brin_demo.gba", "rb");
	fopen_s(&bios, "GBA-emulator/GBA.BIOS", "rb");
	if (fread(GamePak, 0x2000000, 1, file) != 0)
		return 1;
	if (fread(systemROM.getMemoryPtr(), 0x3fff, 1, bios) == 0)
		return 1;

	uint64_t refreshRate = 0;
	uint64_t vCounterDrawCycles = 0;
	uint32_t prevAddr = 0;
	cycles = 0;
	debug = false;

	while (true){
		//at startup
		if (debug || (refreshRate > 100000)){
			debugView->handleEvents();
#if MEMORY_VIEWER
			memoryViewer.handleEvents();
#endif

#if PALETTE_VIEWER
			paletteViewer.handleEvents();
#endif
		}
		if (debug && !step){
			continue;
		}
		step = false;

		if (r[TRegisters::EProgramCounter] == 0x8000954){ //0x8006668, 0x801d6a2
			//debug = true;
		}
		//updateInstructionCycleTimings(*r[PC]);
		uint32_t opCode = cpsr.thumb ? loadFromAddress16(r[TRegisters::EProgramCounter], true) : loadFromAddress32(r[TRegisters::EProgramCounter], true);

		if (debug){
			cout << hex << r[15] << " opCode: " << (cpsr.thumb ? opCode & 0xFFFF : opCode) << " ";
			cout << "r0: " << r[0] << " r1: " << r[1] << " r2: " << r[2] << " r3: " << r[3] << " r4: " << r[4] << " r5: " << r[5] << " r6: " << r[6] << " r7: " << r[7] << " r8: " << r[8] << " r9: " << r[9] << " r10: " << r[10] << " FP (r11): " << r[11] << " IP (r12): " << r[12] << " SP: " << r[13] << " LR: " << r[14] << " CPRS: " << cpsr.val << " SPRS: " << r[16]<< " ";
		}

		cpsr.thumb ? thumbExecute(opCode) : ARMExecute(opCode);

		if (debug){
			cout << endl;
		}

		cycles = 1;

		if (debug || (refreshRate > 100000)){
			debugView->updatePalettes();
#if MEMORY_VIEWER
			memoryViewer.renderMemory();
#endif
#if PALETTE_VIEWER
			paletteViewer.renderPalettes();
#endif
			refreshRate = 0;
		}

		refreshRate += cycles;
		vCounterDrawCycles += cycles;

		if (vCounterDrawCycles >= 240){
			memoryLayout[4][6]++;
			vCounterDrawCycles -= 240;

			if (LCDStatus->LYC == memoryLayout[4][6] && LCDStatus->VcounterIRQEn && InterruptEnableRegister->vCounter){
				InterruptFlagRegister->vCounter = 1;
				LCDStatus->vCounter = 1;
				//debug = true;
			}

			if (memoryLayout[4][6] > 227)
				memoryLayout[4][6] = 0;
		}

		hBlankCounter += cycles;
		if (hBlankCounter >= 1232 && !LCDStatus->vblankFlag){
			hBlankCounter -= 1232;
			LCDStatus->hblankFlag = 0;
			if (InterruptEnableRegister->hBlank && LCDStatus->hIRQEn){
				InterruptFlagRegister->hBlank = 1;
			}
		}
		else if (hBlankCounter > 960)
			LCDStatus->hblankFlag = 1;

		vBlankCounter += cycles;
		if (vBlankCounter >= 280896){
			vBlankCounter -= 280896;
			LCDStatus->vblankFlag = 0;
			if (InterruptEnableRegister->vBlank && LCDStatus->vIRQEn){
				InterruptFlagRegister->vBlank = 1;
			}
		}
		else if (vBlankCounter > 197120)
			LCDStatus->vblankFlag = 1;

		startDMA();
		updateTimers(cycles);
		HWInterrupts(cycles);

		if (debug){
			//std::cout << hex << "r0: " << *r[0] << " r1: " << *r[1] << " r2: " << *r[2] << " r3: " << *r[3] << " r4: " << *r[4] << " r5: " << *r[5] << " r6: " << *r[6] << " r7: " << *r[7] << " r8: " << *r[8] << " r9: " << *r[9] << " r10: " << *r[10] << " FP (r11): " << *r[11] << " IP (r12): " << *r[12] << " SP: " << *r[13] << " LR: " << *r[14] << " CPRS: " << cpsr.val << " SPRS: " << *r[16];
			//std::cout << " cycles " << dec << cycles << std::endl;
			//std::cout << std::endl;
		}

		cycles = 0;
	}

	return 0;
}

