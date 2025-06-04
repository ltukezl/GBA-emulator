#ifndef PushRegisters_H
#define PushRegisters_H

#include <bit>
#include <string>
#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

class PushRegisters
{
public:
	struct PushRegistersOpcode
	{
		uint16_t regList : 8;
		uint16_t PCRLBit : 1;
		uint16_t reserved : 7;
	};

	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return 0;
	}


	static constexpr PushRegistersOpcode fromOpcode(const uint16_t opcode)
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
		return (opcodeStruct.reserved == 0b1011010);
	}

	static inline void push(Registers& regs, const uint32_t value)
	{
		regs[EStackPointer] -= 4;
		writeToAddress32(regs[EStackPointer], value);
	}

	template<uint16_t iterOpcode>
	static void execute(Registers& regs, const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);

		if (op.PCRLBit)
		{
			push(regs, regs[ELinkRegisterLR]);
		}
		for (size_t i = 0; i < 8; i++)
		{
			if ((op.regList << i) & 0x80)
				push(regs, regs[7 - i]);
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
			baseString += "LR";
		}

		return std::format("PUSH {{{}}}", baseString);
	}
};

#endif