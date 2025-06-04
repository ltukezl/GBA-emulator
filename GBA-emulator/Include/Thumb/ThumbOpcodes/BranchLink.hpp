#ifndef BranchLink_H
#define BranchLink_H

#include <bit>
#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

class BranchLink
{
public:
	struct BranchLinkOpcode
	{
		uint16_t offset : 11;
		uint16_t h : 1;
		uint16_t reserved;
	};

	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return opcode & (0x1 << 11);
	}


	static constexpr BranchLinkOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.offset = static_cast<uint16_t>(opcode & 0x07FF),             // bits 0–10
			.h = static_cast<uint16_t>((opcode >> 11) & 0x0001),          // bit 11
			.reserved = static_cast<uint16_t>((opcode >> 12) & 0x000F)    // bits 12–15
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved == 0xF);
	}

	static auto calculateLabelAddr(const uint32_t start, const uint16_t opcode)
	{
		const uint16_t lower = loadFromAddress16(start, true);
		const auto op_h = fromOpcode(opcode);
		const auto op_l = fromOpcode(lower);

		uint32_t result = 0;
		uint32_t val = op_h.offset << 21;
		val = static_cast<uint32_t>(static_cast<int32_t>(val) >> 9);
		result = val + start + 2;
		result += (op_l.offset << 1);
		return result;
	}

	template<uint16_t iterOpcode>
	static void execute(Registers& regs, const uint16_t opcode)
	{
		constexpr auto c_op = fromOpcode(iterOpcode);
		const auto op = fromOpcode(opcode);

		if constexpr (c_op.h == 0)
		{
			uint32_t val = op.offset << 21;
			val = static_cast<uint32_t>(static_cast<int32_t>(val) >> 9);
			regs[ELinkRegisterLR] = val + regs[EProgramCounter] + 2;
		}
		else
		{
			uint32_t nextInstruction = regs[EProgramCounter] + 1;
			regs[EProgramCounter] = regs[ELinkRegisterLR] + (op.offset << 1);
			regs[ELinkRegisterLR] = nextInstruction | 1;
		}
	}

	static auto disassemble(Registers& regs, const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		if(op.h == 0)
			return std::format("BL {:x}", calculateLabelAddr(regs[EProgramCounter], opcode));
		return std::format("");
	}
};

#endif