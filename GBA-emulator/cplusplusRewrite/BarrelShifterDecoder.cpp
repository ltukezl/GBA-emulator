#include "cplusplusRewrite/BarrelShifterDecoder.h"
#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/dataProcessingOp.h"
#include "cplusplusRewrite/SingleDataTransfer.h"

RotatorUnits* BarrelShifterDecoder::decode(DataProcessingOpcode& opCode){
	if (opCode.m_opCode.isImmediate)
		return new ImmediateRotater(m_registers, opCode.m_opCode.immediate);
	else if (opCode.m_opCode.immediate & 0x10)
		return new RegisterWithRegisterShifter(m_registers, opCode.m_opCode.immediate);
	else
		return new RegisterWithImmediateShifter(m_registers, opCode.m_opCode.immediate);
}

RotatorUnits* BarrelShifterDecoder::decode(SingleDataTransfer& opCode) {
	if (opCode.m_opCode.offset & 0x10)
		return new RegisterWithRegisterShifter(m_registers, opCode.m_opCode.offset);
	else
		return new RegisterWithImmediateShifter(m_registers, opCode.m_opCode.offset);
}