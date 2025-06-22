#ifndef UndefOp_H
#define UndefOp_H

#include <bit>
#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"

class UndefOp
{
public:
	struct UndefOpOpcode
	{
		uint32_t unused : 4;
		uint32_t reserved1 : 1;
		uint32_t unsused2 : 20;
		uint32_t reserved2 : 3;
		uint32_t condition : 4;
	};

	static constexpr uint32_t mask(const uint32_t opcode)
	{
		return 0;
	}

	static constexpr UndefOpOpcode fromOpcode(const uint32_t opcode)
	{
		return {
			.unused = static_cast<uint32_t>(opcode & 0xF),               // bits 0–3
			.reserved1 = static_cast<uint32_t>((opcode >> 4) & 0x1),        // bit 4
			.unsused2 = static_cast<uint32_t>((opcode >> 5) & 0xFFFFF),    // bits 5–24
			.reserved2 = static_cast<uint32_t>((opcode >> 25) & 0x7),       // bits 25–27
			.condition = static_cast<uint32_t>((opcode >> 28) & 0xF)        // bits 28–31
		};
	}

	static constexpr bool isThisOpcode(const uint32_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved1 == 1) && (opcodeStruct.reserved2 == 3);
	}

	static void execute(Registers& regs, const uint32_t opcode)
	{

	}

	static auto disassemble(const uint32_t opcode)
	{
		return "Undefined";
	}
};

#endif