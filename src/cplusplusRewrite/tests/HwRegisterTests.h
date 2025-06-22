#pragma once
#include <cassert>

#include "cplusplusRewrite/HwRegisters.h"

void testRegister() {
	auto reg = Registers();
	auto empty = Registers();

	reg[8] = 0x1000;

	reg.updateMode(EUSR);
	assert(reg == Registers({ 0,0,0,0,0,0,0,0,0x1000,0,0,0,0,0,0,0,0,0 }, EUSR));
	assert(reg.getPreviousMode() == ESUPER);

	reg.updateMode(EFIQ);
	assert(reg == empty);
	assert(reg.getPreviousMode() == EUSR);

	reg.updateMode(EUSR);
	assert(reg == Registers({ 0,0,0,0,0,0,0,0,0x1000,0,0,0,0,0,0,0,0,0 }, EUSR));
	assert(reg.getPreviousMode() == EFIQ);
}