#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/Shifts.h"
#include "cplusplusRewrite/dataProcessingOp.h"
#include "GBAcpu.h"
#include <iostream>

RotatorUnits::RotatorUnits(){
	m_shifts[0] = new Lsl(cpsr);
	m_shifts[1] = new Lsr(cpsr);
	m_shifts[2] = new Asr(cpsr);
	m_shifts[3] = new Ror(cpsr);
}
RotatorUnits::~RotatorUnits(){
	delete m_shifts[0];
	delete m_shifts[1];
	delete m_shifts[2];
	delete m_shifts[3];

	m_shifts[0] = nullptr;
	m_shifts[1] = nullptr;
	m_shifts[2] = nullptr;
	m_shifts[3] = nullptr;
}

//------------

ImmediateRotater::ImmediateRotater(uint16_t immediate) {
	immediateRotaterFields.val = immediate;
	m_val = immediate;
}
ImmediateRotater::ImmediateRotater(uint16_t immediate, uint16_t rotateAmount) {
	immediateRotaterFields.immediate = immediate;
	immediateRotaterFields.rotateAmount = rotateAmount;
	m_val = immediateRotaterFields.val;
}

uint32_t ImmediateRotater::calculate(bool setStatus) {
	uint32_t tempResult = 0;
	auto ror = Ror(cpsr);
	ror.execute(tempResult, immediateRotaterFields.immediate, immediateRotaterFields.rotateAmount, setStatus);
	ror.execute(tempResult, immediateRotaterFields.immediate, immediateRotaterFields.rotateAmount, setStatus);
	return tempResult;
}

//------------

RegisterWithImmediateShifter::RegisterWithImmediateShifter(uint16_t val) { 
	registerRotaterFields.val = val;
	m_val = val; 
}

RegisterWithImmediateShifter::RegisterWithImmediateShifter(uint16_t sourceRegister, Rotation rotation, uint16_t shiftAmount) {
	registerRotaterFields.sourceRegister = sourceRegister;
	registerRotaterFields.type = 0;
	registerRotaterFields.shiftCode = rotation;
	registerRotaterFields.shiftAmount = shiftAmount;
	m_val = registerRotaterFields.val;
}
	
uint32_t RegisterWithImmediateShifter::calculate(bool setStatus) {
	uint32_t tmpResult = 0;
	m_shifts[registerRotaterFields.shiftCode]->execute(tmpResult, *r[registerRotaterFields.sourceRegister], registerRotaterFields.shiftAmount, setStatus);
	return tmpResult;
}

//------------


RegisterWithRegisterShifter::RegisterWithRegisterShifter(uint16_t val) { 
	registerRotaterFields.val = val; 
	m_val = val; 
}

RegisterWithRegisterShifter::RegisterWithRegisterShifter(uint16_t sourceRegister, Rotation rotation, uint16_t shiftRegister) {
	registerRotaterFields.sourceRegister = sourceRegister;
	registerRotaterFields.type = 1;
	registerRotaterFields.shiftCode = rotation;
	registerRotaterFields.shiftRegister = shiftRegister;
	m_val = registerRotaterFields.val;
}

uint32_t RegisterWithRegisterShifter::calculate(bool setStatus) {
	uint32_t tmpResult = 0;
	m_shifts[registerRotaterFields.shiftCode]->execute(tmpResult, *r[registerRotaterFields.sourceRegister], *r[registerRotaterFields.shiftRegister], setStatus);
	return tmpResult;
}

//------------
