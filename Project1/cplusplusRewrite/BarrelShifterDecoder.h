#pragma once

#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/dataProcessingOp.h"

class BarrelShifterDecoder {
public:
	BarrelShifterDecoder();

	RotatorUnits* decode(DataProcessingOpcode& opCode);
};