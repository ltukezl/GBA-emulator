#ifndef LoadAddress_H
#define LoadAddress_H

#include <array>
#include <bit>
#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"

class LoadAddress
{
public:
	struct LoadAddressOpcode
	{
		uint16_t immediate : 8;
		uint16_t destination : 3;
		uint16_t useSP : 1;
		uint16_t reserved : 4;
	};

	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return opcode & (0x1 << 11);
	}


	static constexpr LoadAddressOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.immediate = static_cast<uint16_t>(opcode & 0x00FF),             // bits 0–7
			.destination = static_cast<uint16_t>((opcode >> 8) & 0x0007),    // bits 8–10
			.useSP = static_cast<uint16_t>((opcode >> 11) & 0x0001),         // bit 11
			.reserved = static_cast<uint16_t>((opcode >> 12) & 0x000F)       // bits 12–15
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved == 0b1010);
	}

	template<uint16_t iterOpcode>
	static void execute(Registers& regs, const uint16_t opcode)
	{
		constexpr auto c_op = fromOpcode(iterOpcode);
		const auto op = fromOpcode(opcode);

		uint32_t val = 0;
		if constexpr (c_op.useSP)
		{
			val = regs[TRegisters::EStackPointer];
		}
		else
		{
			val = (regs[TRegisters::EProgramCounter] + 2) & ~2;
		}
		regs[op.destination] = (op.immediate << 2) + val;
	}

	static auto disassemble(const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		if (op.useSP)
			return std::format("ADD R{}, SP, #0x{:x}", op.destination, op.immediate << 2);
		return std::format("ADD R{}, PC, #0x{:x}", op.destination, op.immediate << 2);
	}
};

#endif