#include "memoryMappedIO.h"
#include "MemoryOps.h"

union DISPCNT* displayCtrl = (DISPCNT*)&IoRAM[0x0];
union LCDstatus* LCDStatus = (LCDstatus*)&IoRAM[0x4];
union BGoffset* BG0HOFS = (BGoffset*)&IoRAM[0x10];

union KEYCNT* keypadInterruptCtrl = (KEYCNT*)&IoRAM[0x132];
union KEYINPUT* keyInput = (KEYINPUT*)&IoRAM[0x130];

union InterruptEnableRegister_t* InterruptEnableRegister = (InterruptEnableRegister_t*)&IoRAM[0x200];
union InterruptFlagRegister_t* InterruptFlagRegister = (InterruptFlagRegister_t*)&IoRAM[0x202];
union WAITCNT* waitStateControl = (WAITCNT*)&IoRAM[0x204];
union InterruptMaster_t* InterruptMaster = (InterruptMaster_t*)&IoRAM[0x208];

union TIMERCNT* TIMERCNT;

union DMAcontrol* DMAcontrol;

