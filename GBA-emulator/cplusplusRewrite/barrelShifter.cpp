#include <bit>
#include <iostream>

#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/Shifts.h"
#include "cplusplusRewrite/dataProcessingOp.h"
#include "cplusplusRewrite/HwRegisters.h"


//------------

uint32_t ImmediateRotater::calculate(const uint32_t opcode, CPSR_t& cpsr, bool setStatus) {
	const auto immediateRotaterFields = std::bit_cast<ImmediateRotateBits>(opcode);
	const uint32_t tempResult = shifts::Ror::shift(immediateRotaterFields.immediate, immediateRotaterFields.shift);
	const uint32_t tempResult2 = shifts::Ror::shift(tempResult, immediateRotaterFields.shift);
	if (setStatus)
	{
		shifts::Ror::calcConditions(cpsr, tempResult2, tempResult, immediateRotaterFields.shift);
	}
	return tempResult2;
}

//------------
/*
RegisterWithImmediateShifter::~RegisterWithImmediateShifter() = default;

RegisterWithImmediateShifter::RegisterWithImmediateShifter(Registers& registers, uint16_t val) : RotatorUnits(registers) {
	registerRotaterFields.val = val;
	m_val = val; 
}

RegisterWithImmediateShifter::RegisterWithImmediateShifter(Registers& registers, uint16_t sourceRegister, Rotation rotation, uint16_t shiftAmount) : RotatorUnits(registers) {
	registerRotaterFields.sourceRegister = sourceRegister;
	registerRotaterFields.type = 0;
	registerRotaterFields.shiftCode = rotation;
	registerRotaterFields.shiftAmount = shiftAmount;
	m_val = registerRotaterFields.val;
}  
	
uint32_t RegisterWithImmediateShifter::calculate(bool setStatus) {
	uint32_t tmpResult = 0;	
	// TODO: registerRotaterFields -> m_opcodeRotaterField   // päätä itse mutta jos et tätä muuta, mass replace!!
	uint32_t tmpOperand = m_registers[registerRotaterFields.sourceRegister];
	if((registerRotaterFields.shiftAmount == 0) && (shifter() != LSL))
		m_shifts[shifter()]->execute(tmpResult, m_registers[registerRotaterFields.sourceRegister], 0x20, setStatus);
	else
		m_shifts[shifter()]->execute(tmpResult, m_registers[registerRotaterFields.sourceRegister], registerRotaterFields.shiftAmount, setStatus);
	return tmpResult;
}

//------------

RegisterWithRegisterShifter::~RegisterWithRegisterShifter() {
	m_registers[EProgramCounter] -= 4;
}

RegisterWithRegisterShifter::RegisterWithRegisterShifter(Registers& registers, uint16_t val) : RotatorUnits(registers) {
	registerRotaterFields.val = val; 
	m_val = val; 
	m_registers[EProgramCounter] += 4;
}

RegisterWithRegisterShifter::RegisterWithRegisterShifter(Registers& registers, uint16_t sourceRegister, Rotation rotation, uint16_t shiftRegister) : RotatorUnits(registers) {
	registerRotaterFields.sourceRegister = sourceRegister;
	registerRotaterFields.type = 1;
	registerRotaterFields.shiftCode = rotation;
	registerRotaterFields.shiftRegister = shiftRegister;
	m_val = registerRotaterFields.val;
	m_registers[EProgramCounter] += 4;
}

uint32_t RegisterWithRegisterShifter::calculate(bool setStatus) {
	uint32_t tmpResult = 0;
	// If a register is used to specify the shift amount the PC will be 4 bytes ahead.
	
	m_shifts[registerRotaterFields.shiftCode]->execute(tmpResult, m_registers[registerRotaterFields.sourceRegister], m_registers[registerRotaterFields.shiftRegister], setStatus);
	
	return tmpResult;
}

//------------
*/