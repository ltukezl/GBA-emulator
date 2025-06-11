#ifndef MultipleLoad_H
#define MultipleLoad_H

#include <cstdint>
#include <format>

#include "CommonOperations/GbaStrings.hpp"
#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

class MultipleLoad
{
public:
	struct MultipleLoadOpcode
	{
		uint16_t rlist : 8;
		uint16_t baseReg : 3;
		uint16_t reserved : 5;
	};

	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return 0;
	}

	static constexpr MultipleLoadOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.rlist = static_cast<uint16_t>(opcode & 0x00FF),               // bits 0–7
			.baseReg = static_cast<uint16_t>((opcode >> 8) & 0x0007),     // bits 8–11
			.reserved = static_cast<uint16_t>((opcode >> 11) & 0x001F)      // bits 12–15
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved == 0b11001);
	}

	static void execute(Registers& regs, const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		if (op.rlist == 0)
		{
			regs[EProgramCounter] = (loadFromAddress32(regs[op.baseReg]));
			regs[op.baseReg] += 0x40;
		}
		else
		{
			const bool rInList = op.rlist & (1 << op.baseReg);
			uint32_t readAddress = regs[op.baseReg];
			for (size_t i = 0; i < 8; i++)
			{
				if ((op.rlist >> i) & 1)
				{
					regs[i] = loadFromAddress32(readAddress);
					readAddress += 4;
				}
			}

			if (!rInList)
				regs[op.baseReg] = readAddress;
		}
	}

	static auto disassemble(const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		const auto baseString = create_rlist(op.rlist, 8);

		return std::format("LDMIA R{}!, {{{}}}", op.baseReg, baseString);
	}
};

#endif