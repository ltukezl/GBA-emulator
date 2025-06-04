#ifndef PopRegisters_H
#define PopRegisters_H

#include <bit>
#include <string>
#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

class PopRegisters
{
public:
	struct PopRegistersOpcode
	{
		uint16_t regList : 8;
		uint16_t PCRLBit : 1;
		uint16_t reserved : 7;
	};

	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return 0;
	}


	static constexpr PopRegistersOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.regList = static_cast<uint16_t>(opcode & 0xFF),                    
			.PCRLBit = static_cast<uint16_t>((opcode >> 8) & 0x1),              
			.reserved = static_cast<uint16_t>((opcode >> 9) & 0x7F)   
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved == 0b1011110);
	}

	static inline uint32_t pop(Registers& regs)
	{
		const uint32_t value = loadFromAddress32(regs[EStackPointer]);
		regs[EStackPointer] += 4;
		return value;
	}

	template<uint16_t iterOpcode>
	static void execute(Registers& regs, const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		for (size_t i = 0; i < 8; i++)
		{
			if (op.regList & (1 << i))
			{
				regs[i] = pop(regs);
			}
		}
		if (op.PCRLBit)
		{
			regs[EProgramCounter] = pop(regs) & -2;
		}
	}

	static auto createRangeString(uint32_t s, uint32_t e)
	{
		return std::format("R{}-R{}", s, e);
	}

	static auto createPlainReg(uint32_t s)
	{
		return std::format("R{}", s);
	}


	static auto disassemble(const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		std::string baseString = "";
		for (size_t i = 0; i < 8;)
		{
			if (!(op.regList & (1 << i)))
			{
				++i;
				continue;
			}

			// Found the start of a run
			size_t start = i;
			while (i < 8 && (op.regList & (1 << i)))
				++i;

			// Generate string based on run length
			if (!baseString.empty())
				baseString += ",";

			if (start == i - 1)
				baseString += createPlainReg(start);
			else
				baseString += createRangeString(start, i - 1);
		}

		if (op.PCRLBit)
		{
			if (!baseString.empty())
				baseString += ",";
			baseString += "PC";
		}

		return std::format("POP {{{}}}", baseString);
	}
};

#endif