#ifndef UnconditionalBranch_H
#define UnconditionalBranch_H

#include <bit>
#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"

class UnconditionalBranch
{
public:
	struct UnconditionalBranchOpcode
	{
		uint16_t offset : 11;
		uint16_t reserved : 5;
	};

	static constexpr UnconditionalBranchOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.offset = static_cast<uint16_t>(opcode & 0x07FF),             // bits 0–10
			.reserved = static_cast<uint16_t>((opcode >> 11) & 0x001F)    // bits 12–15
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved == 0b11100);
	}

	static void execute(Registers& regs, const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		uint32_t val = op.offset << 21;
		val = static_cast<uint32_t>(static_cast<int32_t>(val) >> 20);
		regs[EProgramCounter] += val + 2;
	}

	static auto calculateLabelAddr(const uint32_t start, const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		uint32_t val = op.offset << 21;
		val = static_cast<uint32_t>(static_cast<int32_t>(val) >> 20);
		return start + val + 2;
	}

	static auto disassemble(const Registers& regs, const uint16_t opcode)
	{
		return std::format("BL {:x}", calculateLabelAddr(regs[EProgramCounter], opcode));
	}
};

#endif