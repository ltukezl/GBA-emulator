#include <cstdint>

#include "cplusplusRewrite/HwRegisters.h"
#include "cplusplusRewrite/MathOps.h"

using namespace mathOps;

static bool negative(const uint32_t result)
{
	return static_cast<int32_t>(result) < 0;
}

static bool zero(const uint32_t result)
{
	return result == 0;
}

static bool addCarry(const uint32_t operand1, const uint32_t operand2, const uint32_t carry)
{
	const auto a = static_cast<uint64_t>(operand1);
	const auto b = static_cast<uint64_t>(operand2);
	const auto c = a + b + carry;

	return (c >> 32) & 1;
}

static bool addOverflow(const uint32_t operand1, const uint32_t operand2, const uint32_t result)
{
	return static_cast<int32_t>((operand1 ^ result) & (operand2 ^ result)) < 0;
}

static bool subCarry(const uint32_t operand1, const uint32_t operand2, const uint32_t result)
{
	return operand1 <= operand2;
}

static bool subOverflow(const uint32_t operand1, const uint32_t operand2, const uint32_t result)
{
	return static_cast<int32_t>((operand1 ^ operand2) & (operand2 ^ result)) < 0;
}


//------------

uint32_t Add::calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2)
{
	return operand1 + operand2;
}

void Add::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t operand1, const uint32_t operand2)
{
	cpsr.zero = zero(result);
	cpsr.negative = negative(result);
	cpsr.carry = addCarry(operand2, operand1, 0);
	cpsr.overflow = addOverflow(operand1, operand2, result);
}

//------------

uint32_t Sub::calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2)
{
	return operand1 - operand2;
}

void Sub::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t operand1, const uint32_t operand2)
{
	cpsr.zero = zero(result);
	cpsr.negative = negative(result);
	cpsr.carry = subCarry(operand2, operand1, result);
	cpsr.overflow = subOverflow(operand2, operand1, result);
}

//------------

uint32_t Mov::calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2)
{
	return operand2;
}

void Mov::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t operand1, const uint32_t operand2)
{
	cpsr.zero = zero(result);
	cpsr.negative = negative(result);
}

//------------

uint32_t Cmp::calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2)
{
	return operand1 - operand2;
}

void Cmp::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t operand1, const uint32_t operand2)
{
	cpsr.zero = zero(result);
	cpsr.negative = negative(result);
	cpsr.carry = subCarry(operand2, operand1, result);
	cpsr.overflow = subOverflow(operand2, operand1, result);
}

//------------

uint32_t And::calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2)
{
	return operand1 & operand2;
}

void And::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t, const uint32_t)
{
	cpsr.zero = zero(result);
	cpsr.negative = negative(result);
}

//------------

uint32_t Eor::calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2)
{
	return operand1 ^ operand2;
}

void Eor::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t, const uint32_t)
{
	cpsr.zero = zero(result);
	cpsr.negative = negative(result);
}

//------------

uint32_t Tst::calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2)
{
	return operand1 & operand2;
}

void Tst::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t, const uint32_t)
{
	cpsr.zero = zero(result);
	cpsr.negative = negative(result);
}

//------------

uint32_t Teq::calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2)
{
	return operand1 ^ operand2;
}

void Teq::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t, const uint32_t)
{
	cpsr.zero = zero(result);
	cpsr.negative = negative(result);
}

//------------

uint32_t Cmn::calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2)
{
	return operand1 + operand2;
}

void Cmn::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t operand1, const uint32_t operand2)
{
	cpsr.zero = zero(result);
	cpsr.negative = negative(result);
	cpsr.carry = addCarry(operand1, operand2, 0);
	cpsr.overflow = addOverflow(operand1, operand2, result);
}

//------------

uint32_t Orr::calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2)
{
	return operand1 | operand2;
}

void Orr::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t, const uint32_t)
{
	cpsr.zero = zero(result);
	cpsr.negative = negative(result);
}

//------------

uint32_t Bic::calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2)
{
	return operand1 & ~operand2;
}

void Bic::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t, const uint32_t)
{
	cpsr.zero = zero(result);
	cpsr.negative = negative(result);
}

//------------

uint32_t Mvn::calculate(const CPSR_t& cpsr, const uint32_t, const uint32_t operand2)
{
	return ~operand2;
}

void Mvn::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t, const uint32_t)
{
	cpsr.zero = zero(result);
	cpsr.negative = negative(result);
}

//------------

uint32_t Mul::calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2)
{
	return (operand1 * operand2) & 0xFFFFFFFF;
}

void Mul::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t, const uint32_t)
{
	cpsr.zero = zero(result);
	cpsr.negative = negative(result);
}

//------------

uint32_t Rsb::calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2)
{
	return operand2 - operand1;
}

void Rsb::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t operand1, const uint32_t operand2)
{
	cpsr.zero = zero(result);
	cpsr.negative = negative(result);
	cpsr.carry = subCarry(operand2, operand1, result);
	cpsr.overflow = subOverflow(operand2, operand1, result);
}

//------------

uint32_t Adc::calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2)
{
	return operand1 + operand2 + cpsr.carry;
}

void Adc::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t operand1, const uint32_t operand2)
{
	cpsr.zero = zero(result);
	cpsr.negative = negative(result);
	cpsr.carry = addCarry(operand2, operand1, cpsr.carry);
	cpsr.overflow = addOverflow(operand2, operand1, result);
}

//------------

uint32_t Sbc::calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2)
{
	return operand1 - operand2 - !cpsr.carry;
}

void Sbc::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t operand1, const uint32_t operand2)
{
	cpsr.zero = zero(result);
	cpsr.negative = negative(result);
	cpsr.carry = subCarry(operand2 + !cpsr.carry, operand1, result);
	cpsr.overflow = subOverflow(operand2, operand1, result);
}

//------------

uint32_t Neg::calculate(const CPSR_t& cpsr, const uint32_t operand1, const uint32_t operand2)
{
	return Rsb::calculate(cpsr, operand2, 0);
}

void Neg::calcConditions(CPSR_t& cpsr, const uint32_t result, const uint32_t operand1, const uint32_t operand2)
{
	Rsb::calcConditions(cpsr, result, 0, operand2);
}
