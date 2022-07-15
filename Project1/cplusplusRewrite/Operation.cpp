#include <stdint.h>
#include "cplusplusRewrite/operation.h"
#include "cplusplusRewrite/barrelShifter.h"
#include "GBAcpu.h"

bool Operation::isZero(uint32_t result) { return result == 0; }
bool Operation::isNegative(int32_t result) { return result < 0; }
bool Operation::additionCarry(uint64_t op1, uint64_t op2, uint64_t carry) {
	uint64_t res = op1 + op2 + carry;
	return ((res >> 32) & 1) == 1;
}
bool Operation::additionOverflow(uint64_t op1, uint64_t op2, uint64_t carry) {
	return cpsr.overflow = ((op1 & op2 & ~carry) | (~op1 & ~op2 & carry)) >> 31 & 1;
}
bool Operation::substractionBorrow(uint64_t op1, uint64_t op2, uint64_t carry) {
	return ((op1 & ~op2) | (op1 & ~carry) | (~op2 & ~carry)) >> 31 & 1;
}
bool Operation::substractionUnderflow(uint64_t op1, uint64_t op2, uint64_t carry) {
	return ((~op1 & op2 & carry) | (op1 & ~op2 & ~carry)) >> 31 & 1;
}

Operation::Operation(union CPSR& programStatus) : m_cpsr(programStatus) {}