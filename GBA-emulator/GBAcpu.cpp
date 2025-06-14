#include "Arm/armopcodes.h"
#include "Constants.h"
#include "Memory/memoryMappedIO.h"
#include <array>
#include <cplusplusRewrite/HwRegisters.h>
#include <cstdint>
#include <Display/Display.h>
#include <Display/Disassembler.hpp>
#include <DMA/DMA.h>
#include <filesystem>
#include <fstream>
#include <Interrupt/interrupt.h>
#include <ios>
#include <Memory/memoryOps.h>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <string>
#include <Thumb/ThumbOpCodes.h>
#include <Timer/timers.h>
#include <vector>

#define BIOS_START 0
#define MEMORY_VIEWER 0
#define PALETTE_VIEWER 0
#define DISASSEMBLER_VIEWER 1

using namespace std;

__int64 vBlankCounter = 0;
__int64 hBlankCounter = 0;

bool hBlankHappened = false;
bool vBlankHappened = false;

Registers r;

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

Display* debugView;

static void readFile(const std::string& fileName, std::vector<unsigned char>& input)
{
	size_t size = std::filesystem::file_size(fileName);

	// Open and read file
	std::ifstream file(fileName, std::ios::binary);

	if (!file.read(reinterpret_cast<char*>(input.data()), size))
	{
		return;
	}
}

static void readFile(const std::string& fileName, std::array<uint8_t, 0x4000>& input)
{
	size_t size = std::filesystem::file_size(fileName);

	// Open and read file
	std::ifstream file(fileName, std::ios::binary);

	if (!file.read(reinterpret_cast<char*>(input.data()), size))
	{
		return;
	}
}

/*
NOTE *r[PC] = 0x08000000 can be used to skip bios check but needs to start in usr mode.
otherwise gba starts from addrs 0 in svc mode
*/
int main(int argc, char *args[]){
#if BIOS_START
	r.m_cpsr.FIQDisable = 1;
	r.m_cpsr.IRQDisable = 1;
	r.m_cpsr.mode = SUPER;
#else
	r.m_cpsr.val = 0x1f;	//current program status register
#endif

	rawWrite16(IoRAM, 0x130, 0xFFFF); // input register, 0 = pressed down, 1 = released

#if DISASSEMBLER_VIEWER 
	Disassembler disassembler;
#endif

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
	r.updateMode(CpuModes_t::ESYS);

#if BIOS_START

#else
	memoryLayout[4][6] = 0x9A; //Vcount initializtion use 7e when cycle counting is ready
#endif
	memoryInits();

	////const std::string game = "GBA-emulator/TestBinaries/FuzzARM.gba";
	//const std::string game = "GBA-emulator/TestBinaries/arm.gba";
	const std::string game = "GBA-emulator/TestBinaries/armwrestler-gba-fixed.gba";
	//const std::string game = "GBA-emulator/TestBinaries/thumb.gba";
	//const std::string game = "GBA-emulator/TestBinaries/program6.bin";
	//const std::string game = "GBA-emulator/TestBinaries/tonc/bigmap.gba";
	//const std::string game = "GBA-emulator/TestBinaries/tonc/obj_demo.gba";
	//const std::string game = "GBA-emulator/TestBinaries/tonc/irq_demo.gba";

	readFile(game, GamePak);
	readFile("GBA-emulator/GBA.BIOS", systemROM.m_memoryArea);

	uint64_t vCounterDrawCycles = 0;
	cycles = 0;
	step = true;

	sf::Clock clock{};
	bool debug = false;
	while (true){
		uint32_t& ProgramCounter = r[TRegisters::EProgramCounter];
#if DISASSEMBLER_VIEWER
		disassembler.display_disassembly(r);
#endif

		uint32_t opCode = r.m_cpsr.thumb ? loadFromAddress16(ProgramCounter, true) : loadFromAddress32(ProgramCounter, true);

		ProgramCounter += r.m_cpsr.thumb ? 2 : 4;
		if (r.m_cpsr.thumb)
			thumbExecute(opCode);
		else
			ARMExecute(opCode);

		cycles = 1;

		vCounterDrawCycles += cycles;

		if (vCounterDrawCycles >= 1232){
			memoryLayout[4][6]++;
			vCounterDrawCycles -= 1232;

			if (LCDStatus->LYC == memoryLayout[4][6] && LCDStatus->VcounterIRQEn && InterruptEnableRegister->vCounter){
				InterruptFlagRegister->vCounter = 1;
				LCDStatus->vCounter = 1;
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
#if DISASSEMBLER_VIEWER
		disassembler.handleEvents();
#endif
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
			debugView->handleEvents();
			debugView->updatePalettes();
#if MEMORY_VIEWER
			memoryViewer.handleEvents();
			memoryViewer.renderMemory();
#endif
#if PALETTE_VIEWER
			paletteViewer.handleEvents();
			paletteViewer.renderPalettes();
#endif
		}
		else if (vBlankCounter > 197120)
			LCDStatus->vblankFlag = 1;

		startDMA();
		updateTimers(cycles);
		HWInterrupts(cycles);
	}

	return 0;
}

