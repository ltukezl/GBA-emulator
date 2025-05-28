#include <cstdint>

#include "cplusplusRewrite/HwRegisters.h"
#include "cplusplusRewrite/Shifts.h"

using namespace shifts;

void Lsl::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount) {
	if (shiftAmount > 32)
		cpsr.carry = 0;
	else if (shiftAmount > 0)
		cpsr.carry = ((unsigned)sourceValue >> (32 - shiftAmount) & 1);
	cpsr.negative = static_cast<int32_t>(result) < 0;
	cpsr.zero = result == 0;
}

uint32_t Lsl::shift(uint32_t sourceValue, uint32_t shiftAmount) {
	uint64_t tmp = sourceValue;
	return static_cast<uint32_t>(tmp << shiftAmount);
}


void Lsr::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount) {
	uint64_t tmp = sourceValue;
	if (shiftAmount > 0)
		cpsr.carry = (tmp >> (shiftAmount - 1) & 1);
	cpsr.negative = static_cast<int32_t>(result) < 0;
	cpsr.zero = result == 0;
}

uint32_t Lsr::shift(const uint32_t sourceValue, const uint32_t shiftAmount) {
	return static_cast<uint64_t>(sourceValue) >> shiftAmount;
}


void Asr::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount) {
	int64_t tmp = static_cast<int64_t>(static_cast<int32_t>(sourceValue));
	if (shiftAmount != 0)
		cpsr.carry = (tmp >> (shiftAmount - 1) & 1);
	cpsr.zero = result == 0;
	cpsr.negative = static_cast<int32_t>(result) < 0;
}

uint32_t Asr::shift(const uint32_t sourceValue, const uint32_t shiftAmount) {
	return static_cast<int64_t>(static_cast<int32_t>(sourceValue)) >> shiftAmount;
}

void Ror::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount) {
	if (shiftAmount > 0)
		cpsr.carry = (sourceValue >> (shiftAmount - 1) & 1);
	cpsr.negative = static_cast<int32_t>(result) < 0;
	cpsr.zero = result == 0;
}

uint32_t Ror::shift(const uint32_t sourceValue, const uint32_t shiftAmount) {
	if (shiftAmount > 32) {
		Ror::shift(sourceValue, shiftAmount - 32);
	}
	return (sourceValue >> shiftAmount) | (sourceValue << (32 - shiftAmount));
}


void Rrx::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount)  {
	cpsr.carry = sourceValue & 1;
	cpsr.negative = static_cast<int32_t>(result) < 0;;
	cpsr.zero = result == 0;
}

uint32_t Rrx::shift(const uint32_t sourceValue, const uint32_t carry) {
	return (carry << 31) | (sourceValue >> 1);
}
