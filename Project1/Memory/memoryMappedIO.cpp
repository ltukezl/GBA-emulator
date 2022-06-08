#include "memoryMappedIO.h"
#include "MemoryOps.h"
#include "Display/RgbaPalette.h"
#include "Display/Tileset.h"

union DISPCNT* displayCtrl = (DISPCNT*)&IoRAM[0x0];
union LCDstatus* LCDStatus = (LCDstatus*)&IoRAM[0x4];
union BGoffset* BG0HOFS = (BGoffset*)&IoRAM[0x10];
union BGoffset* BG0VOFS = (BGoffset*)&IoRAM[0x12];
union BGoffset* BG1HOFS = (BGoffset*)&IoRAM[0x14];
union BGoffset* BG1VOFS = (BGoffset*)&IoRAM[0x16];
union BGoffset* BG2HOFS = (BGoffset*)&IoRAM[0x18];
union BGoffset* BG2VOFS = (BGoffset*)&IoRAM[0x1A];
union BGoffset* BG3HOFS = (BGoffset*)&IoRAM[0x1C];
union BGoffset* BG3VOFS = (BGoffset*)&IoRAM[0x1E];

union KEYCNT* keypadInterruptCtrl = (KEYCNT*)&IoRAM[0x132];
union KEYINPUT* keyInput = (KEYINPUT*)&IoRAM[0x130];

union InterruptEnableRegister_t* InterruptEnableRegister = (InterruptEnableRegister_t*)&IoRAM[0x200];
union InterruptFlagRegister_t* InterruptFlagRegister = (InterruptFlagRegister_t*)&IoRAM[0x202];
union WAITCNT* waitStateControl = (WAITCNT*)&IoRAM[0x204];
union InterruptMaster_t* InterruptMaster = (InterruptMaster_t*)&IoRAM[0x208];

union TIMERCNT* TIMERCNT;

union DMAcontrol* DMAcontrol;

RgbaPalette PaletteColours((ColorPaletteRam*)&PaletteRAM[0]);
Tileset tileset;
