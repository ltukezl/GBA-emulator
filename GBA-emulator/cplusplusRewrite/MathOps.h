#pragma once
#include <cstdint>

#include "cplusplusRewrite/operation.h"

union CPSR_t;

namespace mathOps
{
	class Add
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};

	class Sub
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};

	class Mov
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};

	class Cmp
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};
}