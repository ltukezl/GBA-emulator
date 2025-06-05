#ifndef ConditionalBranch_H
#define ConditionalBranch_H

#include <bit>
#include <cstdint>
#include <format>
#include <cassert>

#include "cplusplusRewrite/HwRegisters.h"
#include "Arm/Conditions.hpp"

class ConditionalBranch
{
public:
	struct ConditionalBranchOpcode
	{
		int16_t offset : 8;
		uint16_t condition : 4;
		uint16_t reserved : 4;
	};

	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return opcode & (0xF << 8);
	}


	static constexpr ConditionalBranchOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.offset = static_cast<int16_t>(opcode & 0x00FF),               // bits 0–7
			.condition = static_cast<uint16_t>((opcode >> 8) & 0x000F),     // bits 8–11
			.reserved = static_cast<uint16_t>((opcode >> 12) & 0x000F)      // bits 12–15
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved == 0b1101);
	}

	template<uint16_t iterOpcode>
	static void execute(Registers& regs, const uint16_t opcode)
	{
		constexpr auto c_op = fromOpcode(iterOpcode);
		const auto op = fromOpcode(opcode);
		assert(op.condition != 15);
		constexpr auto condition_f = conditionspp[c_op.condition];
		if (condition_f(regs))
		{
			const uint32_t location = ((op.offset << 1) + 2);
			regs[EProgramCounter] += location;
		}
	}

	static auto disassemble(const Registers& regs, const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		const auto cond_s = conditionspp_s[op.condition];
		const uint32_t location = ((op.offset << 1) + 2);
		const auto result = regs[EProgramCounter] + location;
		return std::format("{} {:x}", cond_s, result);
	}
};

#endif