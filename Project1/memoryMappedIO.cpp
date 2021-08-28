#include "memoryMappedIO.h"

union InterruptMaster InterruptMaster;
union InterruptEnableRegister InterruptEnableRegister;
union InterruptFlagRegister InterruptFlagRegister;

union ColorPaletteRam ColorPaletteRam;
union BgCnt BgCnt;

union KEYINPUT KEYINPUT;
union KEYCNT KEYCNT;
union TIMERCNT TIMERCNT;

union DMAcontrol DMAcontrol;