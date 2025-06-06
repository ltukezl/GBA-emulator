#ifndef LoadStoreReg_H
#define LoadStoreReg_H

#include <bit>
#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

class LoadStoreReg
{
public:
	struct LoadStoreRegOpcode
	{
		uint16_t destSourceReg : 3;
		uint16_t baseReg : 3;
		uint16_t offsetReg : 3;
		uint16_t reserved1 : 1;
		uint16_t byteSize : 1;
		uint16_t loadBit : 1;
		uint16_t reserved2 : 4;
	};

	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return opcode & (0x3 << 10);
	}

	static constexpr LoadStoreRegOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.destSourceReg = static_cast<uint16_t>(opcode & 0x0007),             // bits 0–2
			.baseReg = static_cast<uint16_t>((opcode >> 3) & 0x0007),            // bits 3–5
			.offsetReg = static_cast<uint16_t>((opcode >> 6) & 0x0007),          // bits 6–8
			.reserved1 = static_cast<uint16_t>((opcode >> 9) & 0x0001),          // bit 9
			.byteSize = static_cast<uint16_t>((opcode >> 10) & 0x0001),          // bit 10
			.loadBit = static_cast<uint16_t>((opcode >> 11) & 0x0001),           // bit 11
			.reserved2 = static_cast<uint16_t>((opcode >> 12) & 0x000F)          // bits 12–15
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved1 == 0) && (opcodeStruct.reserved2 == 0b0101);
	}

	template<uint16_t iterOpcode>
	static void execute(Registers& regs, const uint16_t opcode)
	{
		constexpr auto c_op = fromOpcode(iterOpcode);
		const auto op = fromOpcode(opcode);
		uint32_t totalAddress = regs[op.baseReg] + regs[op.offsetReg];

		if constexpr (c_op.loadBit && c_op.byteSize)
			regs[op.destSourceReg] = loadFromAddress(totalAddress);
		else if constexpr (c_op.loadBit && !c_op.byteSize)
			regs[op.destSourceReg] = loadFromAddress32(totalAddress);
		else if constexpr (!c_op.loadBit && c_op.byteSize)
			writeToAddress(totalAddress, regs[op.destSourceReg]);
		else
			writeToAddress32(totalAddress, regs[op.destSourceReg]);
	}

	static auto disassemble(const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);

		const auto ls = op.loadBit ? "LDR" : "STR";
		const auto bw = op.byteSize ? "B" : "";
		return std::format("{}{} R{}, [R{}, R{}]", ls, bw, op.destSourceReg, op.baseReg, op.offsetReg);
	}
};

#endif