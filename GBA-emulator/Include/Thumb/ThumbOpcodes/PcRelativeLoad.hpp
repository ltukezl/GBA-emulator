#ifndef PcRelativeLoad_H
#define PcRelativeLoad_H

#include <bit>
#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

class PcRelativeLoad
{
public:
	struct PcRelativeLoadOpcode
	{
		uint16_t offset : 8;
		uint16_t destination : 3;
		uint16_t reserved : 5;
	};

	static constexpr PcRelativeLoadOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.offset = static_cast<uint16_t>(opcode & 0xFF),               // bits 0–7
			.destination = static_cast<uint16_t>((opcode >> 8) & 0x7),    // bits 8–10
			.reserved = static_cast<uint16_t>((opcode >> 11) & 0x1F)      // bits 11–15
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved == 9);
	}

	static void execute(Registers& regs, const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		uint32_t tmpPC = (regs[15] + 2) & ~2;
		tmpPC += (op.offset << 2);
		regs[op.destination] = loadFromAddress32(tmpPC);
	}

	static auto disassemble(const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		return std::format("LDR R{}, [PC, #{:x}]", op.destination, op.offset << 2);
	}
};

#endif