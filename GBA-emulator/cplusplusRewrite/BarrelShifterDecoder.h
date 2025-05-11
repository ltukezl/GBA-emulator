#pragma once

#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/dataProcessingOp.h"
#include "cplusplusRewrite/SingleDataTransfer.h"
#include "cplusplusRewrite/HwRegisters.h"

class BarrelShifterDecoder {
private:
	Registers& m_registers;

public:
	BarrelShifterDecoder(Registers& registers) : m_registers(registers) {};

	RotatorUnits* decode(DataProcessingOpcode& opCode);
};