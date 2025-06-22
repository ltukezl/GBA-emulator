#pragma once
#include "cplusplusRewrite/HwRegisters.h"

struct Expected {
	uint32_t result;
	uint32_t cpsr;
};

struct ExpectedReg {
	Registers result;
	uint32_t cpsr;
};