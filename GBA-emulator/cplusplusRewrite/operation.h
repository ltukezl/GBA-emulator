#pragma once
#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/HwRegisters.h"

class Operation {
protected:
	union CPSR_t& m_cpsr;

	bool isZero(uint32_t result);
	bool isNegative(int32_t result);
	bool additionCarry(uint64_t op1, uint64_t op2, uint64_t carry);
	bool substractionBorrow(uint64_t op1, uint64_t op2, uint64_t carry);
	bool additionOverflow(uint64_t op1, uint64_t op2, uint64_t carry);
	bool substractionUnderflow(uint64_t op1, uint64_t op2, uint64_t carry);

public:
	Operation(union CPSR_t& programStatus);
};