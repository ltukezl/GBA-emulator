#pragma once
#include <stdint.h>
//----------------
//Interrupts
//----------------
extern union InterruptMaster{
	struct{
		uint8_t IRQEnabled : 1;
	};
	uint16_t addr;
}InterruptMaster;

extern union InterruptEnableRegister{
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
}InterruptEnableRegister;

extern union InterruptFlagRegister{
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
}InterruptFlagRegister;

//------------------------
//LCD
//------------------------

extern union DISPCNT{
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
}DISPCNT;

extern union ColorPaletteRam{
	struct{
		uint16_t red : 5;
		uint16_t green : 5;
		uint16_t blue : 5;
	};
	uint16_t addr;
}ColorPaletteRam;


extern union BgCnt{
	struct{
		uint16_t priority : 2;
		uint16_t tileBaseBlock : 2;
	    uint16_t :2;
		uint16_t mosaic : 1;
		uint16_t palettes : 1;
		uint16_t bgBaseblock : 5;
		uint16_t overflow : 1;
		uint16_t size : 2;
	};
	uint16_t addr;
}BgCnt;

extern union LCDstatus{
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
}LCDstatus;

//------------------------
//gamepad
//------------------------

extern union KEYINPUT{
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
}KEYINPUT;

extern union KEYCNT {
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
}KEYCNT;

//------------------------
//timers
//------------------------

extern union TIMERCNT {
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
} TIMERCNT;

extern union DMAcontrol{
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
}DMAcontrol;