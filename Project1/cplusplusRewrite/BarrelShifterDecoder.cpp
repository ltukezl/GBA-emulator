#include "cplusplusRewrite/BarrelShifterDecoder.h"
#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/dataProcessingOp.h"

BarrelShifterDecoder::BarrelShifterDecoder() {}

RotatorUnits* BarrelShifterDecoder::decode(DataProcessingOpcode& opCode){
	if (opCode.m_opCode.isImmediate)
		return new ImmediateRotater(opCode.m_opCode.immediate);
	else if (opCode.m_opCode.immediate & 0x10)
		return new RegisterWithRegisterShifter(opCode.m_opCode.immediate);
	else
		return new RegisterWithImmediateShifter(opCode.m_opCode.immediate);
}