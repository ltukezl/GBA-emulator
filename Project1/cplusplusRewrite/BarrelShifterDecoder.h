#pragma once

#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/dataProcessingOp.h"
#include "cplusplusRewrite/SingleDataTransfer.h"

class BarrelShifterDecoder {
public:
	BarrelShifterDecoder();

	RotatorUnits* decode(DataProcessingOpcode& opCode);
	RotatorUnits* decode(SingleDataTransfer& opCode);
};