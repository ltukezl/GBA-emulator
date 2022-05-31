#pragma once
#include <stdint.h>
#include "Display/RgbaPalette.h"
//----------------
//Interrupts
//----------------
union InterruptMaster_t{
	struct{
		uint8_t IRQEnabled : 1;
	};
	uint16_t addr;
};

union InterruptEnableRegister_t{
	struct{
		uint8_t vBlank : 1;
		uint8_t hBlank : 1;
		uint8_t vCounter : 1;
		uint8_t timer0OVF : 1;
		uint8_t timer1OVF : 1;
		uint8_t timer2OVF : 1;
		uint8_t timer3OVF : 1;
		uint8_t serial : 1;
		uint8_t DMA0 : 1;
		uint8_t DMA1 : 1;
		uint8_t DMA2 : 1;
		uint8_t DMA3 : 1;
		uint8_t keyPad : 1;
		uint8_t gamepak : 1;
	};
	uint16_t addr;
};

union InterruptFlagRegister_t{
	struct{
		uint8_t vBlank : 1;
		uint8_t hBlank : 1;
		uint8_t vCounter : 1;
		uint8_t timer0OVF : 1;
		uint8_t timer1OVF : 1;
		uint8_t timer2OVF : 1;
		uint8_t timer3OVF : 1;
		uint8_t serial : 1;
		uint8_t DMA0 : 1;
		uint8_t DMA1 : 1;
		uint8_t DMA2 : 1;
		uint8_t DMA3 : 1;
		uint8_t keyPad : 1;
		uint8_t gamepak : 1;
	};
	uint16_t addr;
};

//------------------------
//LCD
//------------------------

union DISPCNT{
	struct{
		uint16_t bgMode : 3;
		uint16_t CGBMode : 1;
		uint16_t displayFrame : 1;
		uint16_t hBlankInterval : 1;
		uint16_t objectVRAMmap : 1;
		uint16_t forceBlank : 1;
		uint16_t bg0Display : 1;
		uint16_t bg1Display : 1;
		uint16_t bg2Display : 1;
		uint16_t bg3Display : 1;
		uint16_t objDisplay : 1;
		uint16_t win0Display : 1;
		uint16_t win1Display : 1;
		uint16_t objwinDisplay : 1;
	};
	uint16_t addr;
};

union ColorPaletteRam{
	struct{
		uint16_t red : 5;
		uint16_t green : 5;
		uint16_t blue : 5;
	};
	uint16_t addr;
};


union BgCnt{
	struct{
		uint16_t priority : 2;
		uint16_t tileBaseBlock : 2;
	    uint16_t :2;
		uint16_t mosaic : 1;
		uint16_t palettes : 1;
		uint16_t bgBaseblock : 5;
		uint16_t overflow : 1;
		uint16_t hWide : 1;
		uint16_t vWide : 1;
	};
	uint16_t addr;
};

union LCDstatus{
	struct{
		uint16_t vblankFlag : 1;
		uint16_t hblankFlag : 1;
		uint16_t vCounter : 1;
		uint16_t vIRQEn : 1;
		uint16_t hIRQEn : 1;
		uint16_t VcounterIRQEn : 1;
		uint16_t: 2;
		uint16_t LYC : 8;
	};
	uint16_t addr;
};

union BGoffset{
	struct {
		uint16_t offset : 8;
		uint16_t: 8;
	};
	uint16_t addr;
};

union BgTile{
	struct{
		uint16_t tileNumber : 10;
		uint16_t horizontalFlip : 1;
		uint16_t VerticalFlip : 1;
		uint16_t paletteNum : 4;
	};
	uint16_t addr;
};

extern union BGoffset* BG0HOFS;
extern union BGoffset* BG0VOFS;

extern union BGoffset* BG1HOFS;
extern union BGoffset* BG1VOFS;

extern union BGoffset* BG2HOFS;
extern union BGoffset* BG2VOFS;

extern union BGoffset* BG3HOFS;
extern union BGoffset* BG3VOFS;

//------------------------
//obj regs
//------------------------

union ObjReg1{
	struct{
		uint32_t yCoord : 8;
		uint32_t RotOrScale : 1;
		uint32_t isDoubleOrNoDisplay : 1;
		uint32_t objMode : 2;
		uint32_t mosaicMode : 1;
		uint32_t colorMode : 1;
		uint32_t shape : 2;

		uint32_t xCoord : 9;
		uint32_t parameter : 5;
		uint32_t size : 2;
	};
	uint32_t addr;
};

union ObjReg2{
	struct{
		uint16_t tileNumber : 10;
		uint16_t priority : 2;
		uint16_t paletteNumber : 4;
	};
	uint16_t addr;
};

//------------------------
//gamepad
//------------------------

union KEYINPUT{
	struct{
		uint16_t btn_A : 1;
		uint16_t btn_B : 1;
		uint16_t btn_select : 1;
		uint16_t btn_start : 1;
		uint16_t btn_right : 1;
		uint16_t btn_left : 1;
		uint16_t btn_up : 1;
		uint16_t btn_down : 1;
		uint16_t btn_r : 1;
		uint16_t btn_l : 1;
	};
	uint16_t addr;
};

union KEYCNT {
	struct{
		uint16_t btn_A : 1;
		uint16_t btn_B : 1;
		uint16_t btn_select : 1;
		uint16_t btn_start : 1;
		uint16_t btn_right : 1;
		uint16_t btn_left : 1;
		uint16_t btn_up : 1;
		uint16_t btn_down : 1;
		uint16_t btn_r : 1;
		uint16_t btn_l : 1;
		uint16_t unused : 4;
		uint16_t IRQ_EN : 1;
		uint16_t IRQ_cond : 1;
	};
	uint16_t addr;
};

//------------------------
//timers
//------------------------

union TIMERCNT {
	struct{
		uint32_t counterVal : 16;
		uint32_t cntrSelect : 2;
		uint32_t timing : 1;
		uint32_t : 3;
		uint32_t irqEn : 1;
		uint32_t startStop : 1;
		uint32_t: 7;
	};

	uint32_t addr;
};

union DMAcontrol{
	struct{
		uint16_t:5;
		uint16_t destCtrl : 2;
		uint16_t sourceCtrl : 2;
		uint16_t repeat : 1;
		uint16_t transferType : 1;
		uint16_t DRQ : 1;
		uint16_t timing : 2;
		uint16_t irq : 1;
		uint16_t enable : 1;
	};

	uint16_t addr;
};

//------------------------
//mem control
//------------------------

union WAITCNT {
	struct{
		uint16_t sramWait : 2;
		uint16_t waitstate0First : 2;
		uint16_t waitstate0Second : 1;
		uint16_t waitstate1First : 2;
		uint16_t waitstate1Second : 1;
		uint16_t waitstate2First : 2;
		uint16_t waitstate2Second : 1;
	};

	uint16_t addr;
};

extern union DISPCNT* displayCtrl;
extern union LCDstatus* LCDStatus;

extern union KEYCNT* keypadInterruptCtrl;
extern union KEYINPUT* keyInput;

extern union InterruptEnableRegister_t* InterruptEnableRegister;
extern union InterruptFlagRegister_t* InterruptFlagRegister;
extern union InterruptMaster_t* InterruptMaster;

extern union WAITCNT* waitStateControl;
