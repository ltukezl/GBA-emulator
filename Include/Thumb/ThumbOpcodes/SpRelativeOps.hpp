#ifndef SpRelativeOps_H
#define SpRelativeOps_H

#include <bit>
#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

class SpRelativeOps
{
public:
	struct SpRelativeOpsOpcode
	{
		uint16_t immediate : 8;
		uint16_t destSourceReg : 3;
		uint16_t loadFlag : 1;
		uint16_t reserved : 4;
	};

	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return opcode & (1 << 11);
	}

	static constexpr SpRelativeOpsOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.immediate = static_cast<uint16_t>(opcode & 0x00FF),              // bits 0–7
			.destSourceReg = static_cast<uint16_t>((opcode >> 8) & 0x0007),   // bits 8–10
			.loadFlag = static_cast<uint16_t>((opcode >> 11) & 0x0001),       // bit 11
			.reserved = static_cast<uint16_t>((opcode >> 12) & 0x000F)        // bits 12–15
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved == 9);
	}

	template<uint16_t iterOpcode>
	static void execute(Registers& regs, const uint16_t opcode)
	{
		constexpr auto c_op = fromOpcode(iterOpcode);
		const auto op = fromOpcode(opcode);
		if constexpr (c_op.loadFlag)
			regs[op.destSourceReg] = loadFromAddress32(regs[TRegisters::EStackPointer] + (op.immediate << 2));
		else
			writeToAddress32(regs[TRegisters::EStackPointer] + (op.immediate << 2), regs[op.destSourceReg]);
	}

	static auto disassemble(const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		if (op.loadFlag)
			return std::format("LDR R{}, [SP, #0x{:x}]", op.destSourceReg, op.immediate << 2);
		return std::format("STR R{}, [SP, #0x{:x}]", op.destSourceReg, op.immediate << 2);
	}
};

#endif