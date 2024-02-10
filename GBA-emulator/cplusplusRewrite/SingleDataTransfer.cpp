#pragma once
#include <cstdint>
#include "cplusplusRewrite/SingleDataTransfer.h"
#include "cplusplusRewrite/barrelShifterDecoder.h"
#include "cplusplusRewrite/HwRegisters.h"


SingleDataTransfer::SingleDataTransfer(Registers& regs, uint32_t opCode) : m_regs(regs) {
	m_opCode.val = opCode;
}

void SingleDataTransfer::execute() {
	uint32_t operand1 = 0;
	if (m_opCode.immediateOffset)
		operand1 = m_opCode.offset;
	else {
		RotatorUnits* shifter = BarrelShifterDecoder(m_regs).decode(*this); //TODO: move to constructor
		operand1 = shifter->calculate(true);
		delete shifter;
	}
}