#pragma once
#include <cstdint>

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

	class Orr
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};

	class Bic
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};

	class Mvn
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};

	class Cmn
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};

	class Teq
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};

	class Tst
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};

	class Eor
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};

	class And
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};

	class Mul
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};

	class Rsb
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};

	class Adc
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};

	class Sbc
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};

	class Neg
	{
	public:
		static void calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t sourceValue, const uint32_t shiftAmount);
		static uint32_t calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2);
	};
}