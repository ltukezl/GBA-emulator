#include "cplusplusRewrite/Shifts.h"
#include "cplusplusRewrite/HwRegisters.h"

void Lsl::calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) {
	if (shiftAmount > 32)
		m_cpsr.carry = 0;
	else if (shiftAmount > 0)
		m_cpsr.carry = ((unsigned)sourceValue >> (32 - shiftAmount) & 1);
	m_cpsr.negative = result < 0;
	m_cpsr.zero = result == 0;
}

void Lsl::shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) {
	uint64_t tmp = sourceValue;
	destinationRegister = static_cast<uint32_t>(tmp << shiftAmount);
}
Lsl::Lsl(union CPSR_t& programStatus) : ShiferUnit(programStatus) {};

void Lsr::calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) {
	uint64_t tmp = sourceValue;
	if (shiftAmount > 0)
		m_cpsr.carry = (tmp >> (shiftAmount - 1) & 1);
	m_cpsr.negative = result < 0;
	m_cpsr.zero = result == 0;
}

void Lsr::shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) {
	uint64_t tmp = sourceValue;
	destinationRegister = tmp >> shiftAmount;
}
Lsr::Lsr(union CPSR_t& programStatus) : ShiferUnit(programStatus) {};

void Asr::calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) {
	int64_t tmp = (signed)sourceValue;
	if (shiftAmount != 0)
		m_cpsr.carry = (tmp >> (shiftAmount - 1) & 1);
	m_cpsr.zero = result == 0;
	m_cpsr.negative = result < 0;
}

void Asr::shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) {
	int64_t tmp = (signed)sourceValue;
	destinationRegister = tmp >> shiftAmount;
}
Asr::Asr(union CPSR_t& programStatus) : ShiferUnit(programStatus) {};



void Ror::calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) {
	if (shiftAmount > 0)
		m_cpsr.carry = (sourceValue >> (shiftAmount - 1) & 1);
	m_cpsr.negative = result < 0;
	m_cpsr.zero = result == 0;
}

void Ror::shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) {
	if (shiftAmount > 32){
		shift(destinationRegister, sourceValue, shiftAmount % 32);
	}
	else{
		destinationRegister = (sourceValue >> shiftAmount) | (sourceValue << (32 - shiftAmount));
	}
}

Ror::Ror(union CPSR_t& programStatus) : ShiferUnit(programStatus) {};


void Rrx::calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount)  {
	m_cpsr.carry = sourceValue & 1;
	m_cpsr.negative = result < 0;
	m_cpsr.zero = result == 0;

}

void Rrx::shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) {
	destinationRegister = (m_cpsr.carry << 31) | (sourceValue >> 1);
}

Rrx::Rrx(union CPSR_t& programStatus) : ShiferUnit(programStatus) {};