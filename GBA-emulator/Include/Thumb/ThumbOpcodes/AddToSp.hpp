#ifndef AddToSp_H
#define AddToSp_H

#include <array>
#include <bit>
#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"

class AddToSp
{
public:
	struct AddToSpOpcode
	{
		uint16_t imm : 7;
		uint16_t signF : 1;
		uint16_t reserved : 8;
	};

	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return opcode & (1 << 7);
	}


	static constexpr AddToSpOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.imm = static_cast<uint16_t>(opcode & 0x007F),            // bits 0–6
			.signF = static_cast<uint16_t>((opcode >> 7) & 0x0001),   // bit 7
			.reserved = static_cast<uint16_t>((opcode >> 8) & 0x00FF) // bits 8–15
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved == 0b10110000);
	}

	template<uint16_t iterOpcode>
	static void execute(Registers& regs, const uint16_t opcode)
	{
		constexpr auto c_op = fromOpcode(iterOpcode);
		const auto op = fromOpcode(opcode);

		if constexpr (c_op.signF)
		{
			regs[EStackPointer] -= op.imm << 2;
		}
		else
		{
			regs[EStackPointer] += op.imm << 2;
		}
	}

	static auto disassemble(const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		if (op.signF)
			return std::format("ADD SP, #-{:x}", op.imm << 2);
		return std::format("ADD SP, #{:x}", op.imm << 2);
	}
};

#endif