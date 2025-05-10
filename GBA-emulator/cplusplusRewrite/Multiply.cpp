

#include "cplusplusRewrite/multiply.hpp"
#include "cplusplusRewrite/HwRegisters.h"
#include "GBAcpu.h"

void MultiplyLong::execute(Registers& r, const uint32_t opcode)
{
	const auto op = fromOpcode(opcode);
	uint64_t result = 0;

	if (op.sign)
	{
		auto op1 = static_cast<int64_t>(static_cast<int32_t>(r[op.operand1]));
		auto op2 = static_cast<int64_t>(static_cast<int32_t>(r[op.operand2]));
		result = std::bit_cast<uint64_t>(op1 * op2);
	}
	else
	{
		auto op1 = static_cast<uint64_t>(r[op.operand1]);
		auto op2 = static_cast<uint64_t>(r[op.operand2]);
		result = op1 * op2;
	}

	uint32_t hiPart = (result >> 32) & 0xFFFF'FFFF;
	uint32_t loPart = (result >> 0) & 0xFFFF'FFFF;
	r[op.destinationLow] = loPart;
	r[op.destinationHigh] = hiPart;

	if (op.setCondition)
	{
		r.m_cpsr.zero = result ? 0 : 1;
		r.m_cpsr.negative = ((result >> 63) & 1);
	}
}

