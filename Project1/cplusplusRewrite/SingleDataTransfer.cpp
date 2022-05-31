#pragma once
#include <stdint.h>
#include "cplusplusRewrite/SingleDataTransfer.h"
#include "cplusplusRewrite/barrelShifterDecoder.h"


SingleDataTransfer::SingleDataTransfer(uint32_t opCode) {
	m_opCode.val = opCode;
}

SingleDataTransfer::~SingleDataTransfer() {};

void SingleDataTransfer::execute() {
	RotatorUnits* shifter = BarrelShifterDecoder().decode(*this); //TODO: move to constructor


	delete shifter;
}