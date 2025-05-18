#include <cstdint>
#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/operation.h"
#include "cplusplusRewrite/LogicOps.h"
/*
bool LogicOp::isZero(uint32_t result) { return result == 0; }
bool LogicOp::isNegative(int32_t result) { return result < 0; }

void LogicOp::execute(uint32_t& destinationRegister, uint32_t operand1, RotatorUnits& rotation, bool setConditions) {
	uint32_t operand2 = rotation.calculate(setConditions);
	calculate(destinationRegister, operand1, operand2, setConditions);
	if (setConditions)
		calcConditions(operand1, operand2);
}

LogicOp::LogicOp(union CPSR_t& programStatus) : Operation(programStatus) {}

void And::calcConditions(uint32_t op1, uint32_t op2)
{
	m_cpsr.zero = isZero(op1 & op2);
	m_cpsr.negative = isNegative(op1 & op2);
}

void And::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions)
{
	destinationRegister = operand1 & operand2;
}

And::And(CPSR_t& programStatus) : LogicOp(programStatus) {}

//----------

void Or::calcConditions(uint32_t op1, uint32_t op2)
{
	m_cpsr.zero = isZero(op1 | op2);
	m_cpsr.negative = isNegative(op1 | op2);
}

void Or::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions)
{
	destinationRegister = operand1 | operand2;
}

Or::Or(CPSR_t& programStatus) : LogicOp(programStatus) {}

//----------

void Xor::calcConditions(uint32_t op1, uint32_t op2)
{
	m_cpsr.zero = isZero(op1 ^ op2);
	m_cpsr.negative = isNegative(op1 ^ op2);
}

void Xor::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions)
{
	destinationRegister = operand1 ^ operand2;
}

Xor::Xor(CPSR_t& programStatus) : LogicOp(programStatus) {}

//----------

void Tst::calcConditions(uint32_t op1, uint32_t op2)
{
	m_cpsr.zero = isZero(op1 & op2);
	m_cpsr.negative = isNegative(op1 & op2);
}

void Tst::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions)
{
	if (!setConditions)
		destinationRegister = m_cpsr.val;
}

Tst::Tst(CPSR_t& programStatus) : LogicOp(programStatus) {}

// --------

//Mrs.calculate(m_cpsr.m_val, XXX, 0, 0);

void Mrs::calcConditions(uint32_t op1, uint32_t op2)
{
	
}

void Mrs::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions)
{
	if (specialBit)
		destinationRegister = registers[ECPSR];
	else
		destinationRegister = registers[ESavedStatusRegister];
}

void Msr::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions)
{
	if (!setConditions)
		destinationRegister = m_cpsr.val;
}

//----------

void Cmp::calcConditions(uint32_t op1, uint32_t op2)
{
	m_cpsr.zero = isZero(op1 - op2);
	m_cpsr.negative = isNegative(op1 - op2);
	m_cpsr.carry = substractionBorrow(op1, op2, 0);
	m_cpsr.overflow = substractionUnderflow(op1, op2, 0);
}

void Cmp::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions)
{

}

Cmp::Cmp(CPSR_t& programStatus) : LogicOp(programStatus) {}

//----------

void Cmn::calcConditions(uint32_t op1, uint32_t op2)
{
	m_cpsr.zero = isZero(op1 + op2);
	m_cpsr.negative = isNegative(op1 + op2);
	m_cpsr.carry = additionCarry(op1, op2, 0);
	m_cpsr.overflow = additionOverflow(op1, op2, 0);
}

void Cmn::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions)
{
}

Cmn::Cmn(CPSR_t& programStatus) : LogicOp(programStatus) {}

//----------

void Teq::calcConditions(uint32_t op1, uint32_t op2)
{
	m_cpsr.zero = isZero(op1 ^ op2);
	m_cpsr.negative = isNegative(op1 ^ op2);
}

void Teq::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions)
{
	if (!setConditions) {
		m_cpsr.updateAll(operand2);
		//updateMode(); 
	}
}

Teq::Teq(CPSR_t& programStatus) : LogicOp(programStatus) {}

//----------

void Mov::calcConditions(uint32_t op1, uint32_t op2)
{
	m_cpsr.zero = isZero(op2);
	m_cpsr.negative = isNegative(op2);
}

void Mov::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions)
{
	destinationRegister = operand2;
}

Mov::Mov(CPSR_t& programStatus) : LogicOp(programStatus) {}

//----------

void Bic::calcConditions(uint32_t op1, uint32_t op2)
{
	m_cpsr.zero = isZero(op1 & ~op2);
	m_cpsr.negative = isNegative(op1 & ~op2);
}

void Bic::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions)
{
	destinationRegister = operand1 & ~operand2;
}

Bic::Bic(CPSR_t& programStatus) : LogicOp(programStatus) {}

//----------

void Mvn::calcConditions(uint32_t op1, uint32_t op2)
{
	m_cpsr.zero = isZero(~op2);
	m_cpsr.negative = isNegative(~op2);
}

void Mvn::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions)
{
	destinationRegister = ~operand2;
}

Mvn::Mvn(CPSR_t& programStatus) : LogicOp(programStatus) {}

*/