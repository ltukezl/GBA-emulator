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

extern union ColorPaletteRam{
	struct{
		uint16_t red : 5;
		uint16_t green : 5;
		uint16_t blue : 5;
	};
	uint16_t addr;
}ColorPaletteRam;

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