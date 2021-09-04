#include "memoryMappedIO.h"

union InterruptMaster_t InterruptMaster;
union InterruptEnableRegister_t InterruptEnableRegister;
union InterruptFlagRegister_t InterruptFlagRegister = {0};

union ColorPaletteRam ColorPaletteRam;
union BgCnt BgCnt;
union LCDstatus LCDstatus;
union DISPCNT DISPCNT;

union KEYINPUT KEYINPUT;
union KEYCNT KEYCNT;
union TIMERCNT TIMERCNT;

union DMAcontrol DMAcontrol;