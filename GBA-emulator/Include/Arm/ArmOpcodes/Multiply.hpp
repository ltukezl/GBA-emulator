#pragma once

#include <cstdint>
#include <bit>
#include <array>
#include "Constants.h"

#include "cplusplusRewrite/HwRegisters.h"

class MultiplyAccumulate
{
public:
	struct MultiplyAccumulateOpcode
	{
		uint32_t operand1 : 4;
		uint32_t reserved1 : 4;
		uint32_t operand2 : 4;
		uint32_t destinationLow : 4;
		uint32_t destinationHigh : 4;
		uint32_t setCondition : 1;
		uint32_t accumulate : 1;
		uint32_t sign : 1;
		uint32_t reserved2 : 5;
		uint32_t cond : 4;
	};

	static constexpr MultiplyAccumulateOpcode fromOpcode(const uint32_t opcode)
	{
		return std::bit_cast<MultiplyAccumulateOpcode>(opcode);
	}

	static constexpr uint32_t fromFields(const uint32_t operand1, const uint32_t operand2, const uint32_t destinationLow, const uint32_t destinationHigh, const bool setCond, const bool accumulate, const bool sign) {
		MultiplyAccumulateOpcode opcode{};
		opcode.operand1 = operand1;
		opcode.operand2 = operand2;
		opcode.destinationLow = destinationLow;
		opcode.destinationHigh = destinationHigh;
		opcode.cond = setCond;
		opcode.accumulate = accumulate;
		opcode.sign = sign;

		return std::bit_cast<uint32_t>(opcode);
	}

	static constexpr bool isThisOpcode(const uint32_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved1 == 9) && (opcodeStruct.reserved2 == 1);
	}

	static void execute(Registers& regs, const uint32_t opcode)
	{
		const auto op = fromOpcode(opcode);
		uint64_t result = 0;

		if (op.sign)
		{
			const auto op1 = static_cast<int64_t>(static_cast<int32_t>(regs[op.operand1]));
			const auto op2 = static_cast<int64_t>(static_cast<int32_t>(regs[op.operand2]));
			result = std::bit_cast<uint64_t>(op1 * op2);
		}
		else
		{
			const auto op1 = static_cast<uint64_t>(regs[op.operand1]);
			const auto op2 = static_cast<uint64_t>(regs[op.operand2]);
			result = op1 * op2;
		}

		if (op.accumulate)
		{
			uint64_t current = static_cast<uint64_t>(regs[op.destinationHigh]) << 32 | static_cast<uint64_t>(regs[op.destinationLow]);
			result += current;
		}

		const uint32_t hiPart = (result >> 32) & 0xFFFF'FFFF;
		const uint32_t loPart = (result >> 0) & 0xFFFF'FFFF;
		regs[op.destinationLow] = loPart;
		regs[op.destinationHigh] = hiPart;

		if (op.setCondition)
		{
			r.m_cpsr.zero = result ? 0 : 1;
			r.m_cpsr.negative = ((result >> 63) & 1);
		}
	}

	static auto disassemble(const uint32_t opcode)
	{
		const auto op = fromOpcode(opcode);
		static constexpr std::array<std::array<const char*, 2>, 2> txt = { { {"UMULL", "UMLAL"}, { "SMULL", "SMLAL"} } };
		const auto s = op.setCondition ? "S" : "";

		return std::format("{}{}{} R{},R{}, R{},R{}", txt[op.sign][op.accumulate], condition_strings[op.cond], s, op.destinationLow, op.destinationHigh, op.operand1, op.operand2);
	}
};

class MultiplyLong
{
public:
	struct MultiplyLongOp
	{
		uint32_t operand1 : 4;
		uint32_t reserved1 : 4;
		uint32_t operand2 : 4;
		uint32_t operand3 : 4;
		uint32_t destination : 4;
		uint32_t setCondition : 1;
		uint32_t accumulate : 1;
		uint32_t reserved2 : 6;
		uint32_t cond : 4;
	};

	static constexpr MultiplyLongOp fromOpcode(const uint32_t opcode)
	{
		return std::bit_cast<MultiplyLongOp>(opcode);
	}

	static constexpr uint32_t fromFields(const uint32_t operand1, const uint32_t operand2, const uint32_t operand3, const uint32_t destination, const bool setCond, const bool accumulate) {
		MultiplyLongOp opcode{};
		opcode.operand1 = operand1;
		opcode.operand2 = operand2;
		opcode.operand3 = operand3;
		opcode.destination = destination;
		opcode.cond = setCond;
		opcode.accumulate = accumulate;

		return std::bit_cast<uint32_t>(opcode);
	}

	static constexpr bool isThisOpcode(const uint32_t opcode)
	{
		auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved1 == 9) && (opcodeStruct.reserved2 == 0);
	}

	static void execute(Registers& regs, const uint32_t opcode)
	{
		const auto op = fromOpcode(opcode);
		uint64_t result = regs[op.operand1] * regs[op.operand2];
		result += op.accumulate ? regs[op.operand3] : 0;
		regs[op.destination] = result;
		if (op.setCondition) {
			r.m_cpsr.zero = result ? 0 : 1;
			r.m_cpsr.negative = ((result >> 31) & 1);
		}
	}

	static auto disassemble(const uint32_t opcode)
	{
		const auto op = fromOpcode(opcode);
		const auto s = op.setCondition ? "S" : "";

		if(op.accumulate)
			return std::format("MLA{}{} R{},R{},R{},R{}", condition_strings[op.cond], s, op.destination, op.operand1, op.operand2, op.operand3);
		else
			return std::format("MUL{}{} R{},R{},R{}", condition_strings[op.cond], s, op.destination, op.operand1, op.operand2);
	}
};