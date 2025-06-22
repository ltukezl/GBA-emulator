#pragma once

#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

class LoadStoreImm
{
public:
	struct LoadStoreImmOpcode
	{
		uint16_t destSourceReg : 3;
		uint16_t baseReg : 3;
		uint16_t offset : 5;
		uint16_t loadFlag : 1;
		uint16_t byteSize : 1;
		uint16_t unused : 3;
	};

	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return opcode & (0x3 << 11);
	}

	static constexpr LoadStoreImmOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.destSourceReg = static_cast<uint16_t>(opcode & 0x7),               // bits 0–2
			.baseReg = static_cast<uint16_t>((opcode >> 3) & 0x7),              // bits 3–5
			.offset = static_cast<uint16_t>((opcode >> 6) & 0x1F),              // bits 6–10
			.loadFlag = static_cast<uint16_t>((opcode >> 11) & 0x1),            // bit 11
			.byteSize = static_cast<uint16_t>((opcode >> 12) & 0x1),            // bit 12
			.unused = static_cast<uint16_t>((opcode >> 13) & 0x7)               // bits 13–15
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.unused == 3);
	}

	template<uint16_t iterOpcode>
	static void execute(Registers& regs, const uint16_t opcode)
	{
		constexpr auto c_op = fromOpcode(iterOpcode);
		const auto op = fromOpcode(opcode);
		const uint32_t shift = op.byteSize ? 0 : 2;
		const uint8_t immediate = op.offset << shift;
		const uint32_t totalAddress = regs[op.baseReg] + immediate;

		if constexpr (c_op.loadFlag && c_op.byteSize)
			regs[op.destSourceReg] = loadFromAddress(totalAddress);
		else if constexpr (c_op.loadFlag && !c_op.byteSize)
			regs[op.destSourceReg] = loadFromAddress32(totalAddress);
		else if constexpr (!c_op.loadFlag && c_op.byteSize)
			writeToAddress(totalAddress, regs[op.destSourceReg]);
		else
			writeToAddress32(totalAddress, regs[op.destSourceReg]);
	}

	static auto disassemble(const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		const uint32_t shift = op.byteSize ? 0 : 2;
		const uint8_t immediate = op.offset << shift;
		const auto ls = op.loadFlag ? "LDR" : "STR";
		const auto bw = op.byteSize ? "B" : "";
		return std::format("{}{} R{}, [R{}, #0x{:x}]", ls, bw, op.destSourceReg, op.baseReg, immediate);
	}
};
