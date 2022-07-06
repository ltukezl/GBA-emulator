#pragma once
#include <cassert>

#include "cplusplusRewrite/HwRegisters.h"

void testRegister() {
	auto reg = Registers();
	auto empty = Registers();

	reg[8] = 0x1000;

	reg.updateMode(USR);
	assert(reg == Registers({ 0,0,0,0,0,0,0,0,0x1000,0,0,0,0,0,0,0,0,0 }, USR));

	reg.updateMode(FIQ);
	assert(reg == empty);

	reg.updateMode(USR);
	assert(reg == Registers({ 0,0,0,0,0,0,0,0,0x1000,0,0,0,0,0,0,0,0,0 }, USR));	
}