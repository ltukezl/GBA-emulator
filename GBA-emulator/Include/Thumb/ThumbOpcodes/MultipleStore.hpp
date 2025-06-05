#ifndef MultipleStore_H
#define MultipleStore_H

#include <bit>
#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

class MultipleStore
{
public:
	struct MultipleStoreOpcode
	{
		uint16_t rlist : 8;
		uint16_t baseReg : 3;
		uint16_t reserved : 5;
	};

	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return 0;
	}

	static constexpr MultipleStoreOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.rlist = static_cast<uint16_t>(opcode & 0x00FF),               // bits 0–7
			.baseReg = static_cast<uint16_t>((opcode >> 8) & 0x0003),     // bits 8–11
			.reserved = static_cast<uint16_t>((opcode >> 11) & 0x001F)      // bits 12–15
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved == 0b11000);
	}

	static void execute(Registers& regs, const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		const uint32_t bits = std::popcount((op.rlist & ((1ULL << op.baseReg) - 1ULL)));
		const bool first = bits == 0;
		const uint32_t savedAddr = (bits << 2) + regs[op.baseReg];

		if (op.rlist == 0)
		{
			writeToAddress32(regs[op.baseReg], regs[EProgramCounter] + 4);
			regs[op.baseReg] += 0x40;
		}
		else
		{
			for (size_t i = 0; i < 8; i++)
			{
				if (op.rlist & (1 << i))
				{
					writeToAddress32(regs[op.baseReg], regs[i]);
					regs[op.baseReg] += 4;
				}
			}
		}
		if (!first)
			writeToAddress32(savedAddr, regs[op.baseReg]);
	}

	static auto disassemble(const uint16_t opcode)
	{

	}
};

#endif