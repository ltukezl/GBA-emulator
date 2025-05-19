#pragma once

#include "cplusplusRewrite/barrelShifter.h"

class BarrelShifterDecoder {
public:
	decltype(&ImmediateRotater::calculate) static decode(const uint32_t opCode);

private:
	struct DecodeBits
	{
		uint32_t unused : 4;
		uint32_t type : 1;
		uint32_t unused2 : 15;
		uint32_t setConditions : 1;
		uint32_t unused3 : 4;
		uint32_t immediate : 1;
		uint32_t opCode : 2;
		uint32_t : 0;
	};
};