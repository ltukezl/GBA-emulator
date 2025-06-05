#ifndef LoadStoreHalfword_H
#define LoadStoreHalfword_H

#include <bit>
#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

class LoadStoreHalfword
{
public:
	struct LoadStoreHalfwordOpcode
	{
		uint16_t destSourceReg : 3;
		uint16_t baseReg : 3;
		uint16_t offset : 5;
		uint16_t loadFlag : 1;
		uint16_t reserved : 4;
	};

	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return opcode & (1 << 11);
	}

	static constexpr LoadStoreHalfwordOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.destSourceReg = static_cast<uint16_t>(opcode & 0x0007),               // bits 0–2
			.baseReg = static_cast<uint16_t>((opcode >> 3) & 0x0007),              // bits 3–5
			.offset = static_cast<uint16_t>((opcode >> 6) & 0x001F),               // bits 6–10
			.loadFlag = static_cast<uint16_t>((opcode >> 11) & 0x0001),            // bit 11
			.reserved = static_cast<uint16_t>((opcode >> 12) & 0x000F)             // bits 12–15
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved == 0b1000);
	}

	template<uint16_t iterOpcode>
	static void execute(Registers& regs, const uint16_t opcode)
	{
		constexpr auto c_op = fromOpcode(iterOpcode);
		const auto op = fromOpcode(opcode);

		const uint32_t immediate = op.offset << 1;
		if constexpr (c_op.loadFlag)
			regs[op.destSourceReg] = loadFromAddress16(regs[op.baseReg] + immediate);
		else
			writeToAddress16(regs[op.baseReg] + immediate, regs[op.destSourceReg]);
	}

	static auto disassemble(const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		const uint32_t immediate = op.offset << 1;
		if (op.loadFlag)
			return std::format("LRDH R{}, [R{}, #{:x}]", op.destSourceReg, op.baseReg, immediate);
		return std::format("STRH R{}, [R{}, #{:x}]", op.destSourceReg, op.baseReg, immediate);
	}
};

#endif