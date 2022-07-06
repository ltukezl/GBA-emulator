#pragma once
#include <stdint.h>
#include "cplusplusRewrite/SingleDataTransfer.h"
#include "cplusplusRewrite/barrelShifterDecoder.h"


SingleDataTransfer::SingleDataTransfer(uint32_t opCode) {
	m_opCode.val = opCode;
}

SingleDataTransfer::~SingleDataTransfer() {};

void SingleDataTransfer::execute() {
	uint32_t operand1 = 0;
	if (m_opCode.immediateOffset)
		operand1 = m_opCode.offset;
	else {
		RotatorUnits* shifter = BarrelShifterDecoder().decode(*this); //TODO: move to constructor
		operand1 = shifter->calculate(true);
		delete shifter;
	}


	
}