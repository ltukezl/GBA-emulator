#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/Shifts.h"
#include "cplusplusRewrite/dataProcessingOp.h"
#include "cplusplusRewrite/HwRegisters.h"
#include "GBAcpu.h"
#include <iostream>

RotatorUnits::RotatorUnits(Registers& registers) : m_registers(registers) {
	m_shifts[0] = new Lsl(cpsr);
	m_shifts[1] = new Lsr(cpsr);
	m_shifts[2] = new Asr(cpsr);
	m_shifts[3] = new Ror(cpsr);
	m_shifts[4] = new Rrx(cpsr);
	m_val = 0;
}
RotatorUnits::~RotatorUnits(){
	delete m_shifts[0];
	delete m_shifts[1];
	delete m_shifts[2];
	delete m_shifts[3];
	delete m_shifts[4];

	m_shifts[0] = nullptr;
	m_shifts[1] = nullptr;
	m_shifts[2] = nullptr;
	m_shifts[3] = nullptr;
	m_shifts[4] = nullptr;
}

//------------

ImmediateRotater::~ImmediateRotater() {}

ImmediateRotater::ImmediateRotater(Registers& registers, uint16_t immediate) : RotatorUnits(registers) {
	immediateRotaterFields.val = immediate;
	m_val = immediate;
}

ImmediateRotater::ImmediateRotater(Registers& registers, uint16_t immediate, uint16_t rotateAmount) : RotatorUnits(registers) {
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

RegisterWithImmediateShifter::~RegisterWithImmediateShifter() {}

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
