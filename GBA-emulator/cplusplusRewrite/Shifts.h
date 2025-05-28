#pragma once
#include <cstdint>

union CPSR_t;

namespace shifts
{
	class Lsl {
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t shift(const uint32_t sourceValue, const uint32_t shiftAmount);
	};

	class Lsr {
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t shift(const uint32_t sourceValue, const uint32_t shiftAmount);
	};

	class Asr{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t shift(const uint32_t sourceValue, const uint32_t shiftAmount);
	};

	class Ror {
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t shift(const uint32_t sourceValue, const uint32_t shiftAmount);
	};

	class Rrx{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount) ;
		static uint32_t shift(const uint32_t sourceValue, const uint32_t carry);
	};
}