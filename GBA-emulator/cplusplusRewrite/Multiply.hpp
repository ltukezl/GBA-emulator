#pragma once

#include <cstdint>
#include <bit>

class Registers;

class MultiplyAccumulate
{
public:
	struct MulAccOpcode
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

	static constexpr MulAccOpcode fromOpcode(const uint32_t opcode)
	{
		return std::bit_cast<MulAccOpcode>(opcode);
	}

	static constexpr uint32_t fromFields(const uint32_t operand1, const uint32_t operand2, const uint32_t operand3, const uint32_t destination, const bool setCond, const bool accumulate) {
		MulAccOpcode opcode{};
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

	static void execute(Registers& regs, const uint32_t opcode);
};

class MultiplyLong
{
public:
	struct MultiplyLongOpcode
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

	static constexpr MultiplyLongOpcode fromOpcode(const uint32_t opcode)
	{
		return std::bit_cast<MultiplyLongOpcode>(opcode);
	}

	static constexpr uint32_t fromFields(const uint32_t operand1, const uint32_t operand2, const uint32_t destinationLow, const uint32_t destinationHigh, const bool setCond, const bool accumulate, const bool sign) {
		MultiplyLongOpcode opcode{};
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

	static void execute(Registers& regs, const uint32_t opcode);
};