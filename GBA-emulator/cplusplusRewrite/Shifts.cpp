#include <cstdint>

#include "cplusplusRewrite/HwRegisters.h"
#include "cplusplusRewrite/Shifts.h"

using namespace shifts;

void Lsl::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount) {
	const auto maskedShift = shiftAmount & 0xFF;
	if (maskedShift > 32)
		cpsr.carry = 0;
	else if (maskedShift > 0)
		cpsr.carry = ((unsigned)sourceValue >> (32 - shiftAmount) & 1);
	cpsr.negative = static_cast<int32_t>(result) < 0;
	cpsr.zero = result == 0;
}

uint32_t Lsl::shift(const uint32_t sourceValue, const uint32_t shiftAmount) {
	const auto maskedShift = shiftAmount & 0xFF;
	if (maskedShift > 32)
		return 0;
	const uint64_t tmp = sourceValue;
	const auto result = static_cast<uint32_t>(tmp << maskedShift);
	return result;
}


void Lsr::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount) {
	uint64_t tmp = sourceValue;
	uint8_t maskedShift = static_cast<uint8_t>(shiftAmount);
	if (maskedShift > 33)
		maskedShift = 33;
	if (maskedShift > 0)
		cpsr.carry = (tmp >> (maskedShift - 1) & 1);
	cpsr.negative = static_cast<int32_t>(result) < 0;
	cpsr.zero = result == 0;
}

uint32_t Lsr::shift(const uint32_t sourceValue, const uint32_t shiftAmount) {
	uint8_t maskedShift = static_cast<uint8_t>(shiftAmount);
	if (maskedShift > 33)
		maskedShift = 33;
	return static_cast<uint64_t>(sourceValue) >> maskedShift;
}


void Asr::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount) {
	int64_t tmp = static_cast<int64_t>(static_cast<int32_t>(sourceValue));
	uint8_t maskedShift = static_cast<uint8_t>(shiftAmount);
	if (maskedShift > 33)
		maskedShift = 33;

	if (maskedShift != 0)
		cpsr.carry = ((tmp >> (maskedShift - 1)) & 1);
	cpsr.zero = result == 0;
	cpsr.negative = static_cast<int32_t>(result) < 0;
}

uint32_t Asr::shift(const uint32_t sourceValue, const uint32_t shiftAmount) {
	uint8_t maskedShift = static_cast<uint8_t>(shiftAmount);
	if (maskedShift > 33)
		maskedShift = 33;
	return static_cast<int64_t>(static_cast<int32_t>(sourceValue)) >> maskedShift;
}

void Ror::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount) {
	if (static_cast<uint8_t>(shiftAmount) > 0)
		cpsr.carry = (sourceValue >> (shiftAmount - 1) & 1);
	cpsr.negative = static_cast<int32_t>(result) < 0;
	cpsr.zero = result == 0;
}

uint32_t Ror::shift(const uint32_t sourceValue, const uint32_t shiftAmount) {
	const uint32_t effectiveShift = shiftAmount % 32;
	return (sourceValue >> effectiveShift) | (sourceValue << (32 - effectiveShift));
}


void Rrx::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount)  {
	cpsr.carry = sourceValue & 1;
	cpsr.negative = static_cast<int32_t>(result) < 0;;
	cpsr.zero = result == 0;
}

uint32_t Rrx::shift(const uint32_t sourceValue, const uint32_t carry) {
	return (carry << 31) | (sourceValue >> 1);
}
