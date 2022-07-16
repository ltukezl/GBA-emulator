#include <stdint.h>
#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/MathOps.h"
#include "cplusplusRewrite/operation.h"
#include "GBAcpu.h"


void MathOp::execute(uint32_t& destinationRegister, uint32_t operand1, RotatorUnits& rotation, bool setConditions){
	uint32_t operand2 = rotation.calculate(false);
	calculate(destinationRegister, operand1, operand2);
	if (setConditions)
		calcConditions(destinationRegister, operand1, operand2);
}

MathOp::MathOp(union CPSR& programStatus) : Operation(programStatus) {}

//-------
void Addition::calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) {
	m_cpsr.zero = isZero(result);
	m_cpsr.negative = isNegative(result);
	m_cpsr.carry = additionCarry(operand1, operand2, result);
	m_cpsr.overflow = additionOverflow(operand1, operand2, result);
}

void Addition::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) {
	destinationRegister = operand1 + operand2;
}

Addition::Addition(union CPSR& programStatus) : MathOp(programStatus){}

//-------
void Substraction::calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) {
	m_cpsr.zero = isZero(result);
	m_cpsr.negative = isNegative(result);
	m_cpsr.carry = substractionBorrow(operand1, operand2, result);
	m_cpsr.overflow = substractionUnderflow(operand1, operand2, result);
}

void Substraction::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) {
	destinationRegister = operand1 - operand2;
}

Substraction::Substraction(union CPSR& programStatus) : MathOp(programStatus){}

//-------
void ReverseSubstraction::calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) {
	m_cpsr.zero = isZero(result);
	m_cpsr.negative = isNegative(result);
	m_cpsr.carry = substractionBorrow(operand1, operand2, result);
	m_cpsr.overflow = substractionUnderflow(operand1, operand2, result);
}

void ReverseSubstraction::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) {
	destinationRegister = operand2 - operand1;
}

ReverseSubstraction::ReverseSubstraction(union CPSR& programStatus) : MathOp(programStatus){}

//-------
void AddWithCarry::calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) {
	m_cpsr.zero = isZero(result);
	m_cpsr.negative = isNegative(result);
	m_cpsr.carry = additionCarry(operand1, operand2, result);
	m_cpsr.overflow = additionOverflow(operand1, operand2, result);
}

void AddWithCarry::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) {
	destinationRegister = operand1 + operand2 + m_cpsr.carry;
}

AddWithCarry::AddWithCarry(union CPSR& programStatus) : MathOp(programStatus){}

//-------
void BorrowSubstraction::calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) {
	m_cpsr.zero = isZero(result);
	m_cpsr.negative = isNegative(result);
	m_cpsr.carry = substractionBorrow(operand1, operand2, result);
	m_cpsr.overflow = substractionUnderflow(operand1, operand2, result);
}

void BorrowSubstraction::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) {
	destinationRegister = operand1 - operand2 - !m_cpsr.carry;
}

BorrowSubstraction::BorrowSubstraction(union CPSR& programStatus) : MathOp(programStatus){}

//-------
void ReverseBorrowSubstraction::calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) {
	m_cpsr.zero = isZero(result);
	m_cpsr.negative = isNegative(result);
	m_cpsr.carry = substractionBorrow(operand2, operand1, result);
	m_cpsr.overflow = substractionUnderflow(operand2, operand1, result);
}

void ReverseBorrowSubstraction::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) {
	destinationRegister = operand2 - operand1 - !m_cpsr.carry;
}

ReverseBorrowSubstraction::ReverseBorrowSubstraction(union CPSR& programStatus) : MathOp(programStatus){}

//-------
void Negation::calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) {
	m_cpsr.zero = isZero(result);
	m_cpsr.negative = isNegative(result);
	m_cpsr.carry = substractionBorrow(operand1, 0, 0);
	m_cpsr.overflow = substractionUnderflow(operand1, 0, 0);
}

void Negation::calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) {
	destinationRegister = 0 - operand2;
}

Negation::Negation(union CPSR& programStatus) : MathOp(programStatus){}

