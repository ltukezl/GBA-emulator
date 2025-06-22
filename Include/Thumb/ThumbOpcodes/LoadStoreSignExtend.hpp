#ifndef LoadStoreSignExtend_H
#define LoadStoreSignExtend_H

#include <bit>
#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"

class LoadStoreSignExtend
{
public:
	struct LoadStoreSignExtendOpcode
	{
		uint16_t destSourceReg : 3;
		uint16_t baseReg : 3;
		uint16_t offsetReg : 3;
		uint16_t reserved1 : 1;
		uint16_t extend : 1;
		uint16_t halfWord : 1;
		uint16_t reserved2 : 4;
	};

	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return opcode & (0x3 << 10);
	}

	static constexpr LoadStoreSignExtendOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.destSourceReg = static_cast<uint16_t>(opcode & 0x0007),              // bits 0–2
			.baseReg = static_cast<uint16_t>((opcode >> 3) & 0x0007),             // bits 3–5
			.offsetReg = static_cast<uint16_t>((opcode >> 6) & 0x0007),           // bits 6–8
			.reserved1 = static_cast<uint16_t>((opcode >> 9) & 0x0001),           // bit 9
			.extend = static_cast<uint16_t>((opcode >> 10) & 0x0001),             // bit 10
			.halfWord = static_cast<uint16_t>((opcode >> 11) & 0x0001),           // bit 11
			.reserved2 = static_cast<uint16_t>((opcode >> 12) & 0x000F)           // bits 12–15
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved1 == 1) && (opcodeStruct.reserved2 == 0b0101);
	}

	template<uint16_t iterOpcode>
	static void execute(Registers& regs, const uint16_t opcode)
	{
		constexpr auto c_op = fromOpcode(iterOpcode);
		const auto op = fromOpcode(opcode);
		uint32_t totalAddress = regs[op.baseReg] + regs[op.offsetReg];

		if constexpr (c_op.halfWord && !c_op.extend)
			regs[op.destSourceReg] = loadFromAddress16(totalAddress);
		else if constexpr (!c_op.halfWord && !c_op.extend)
			writeToAddress16(totalAddress, regs[op.destSourceReg]);
		else if constexpr (c_op.extend)
		{
			if (c_op.halfWord && !(totalAddress & 1))
			{
				regs[op.destSourceReg] = static_cast<int32_t>(static_cast<int16_t>(loadFromAddress16(totalAddress)));
			}
			else
			{
				regs[op.destSourceReg] = static_cast<int32_t>(static_cast<int8_t>(loadFromAddress(totalAddress)));
			}
		}
	}

	static auto disassemble(const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		if (op.halfWord && op.extend)
			return std::format("LDRSH R{}, [R{}, R{}]", op.destSourceReg, op.baseReg, op.offsetReg);
		if (op.halfWord && !op.extend)
			return std::format("LDRH R{}, [R{}, R{}]", op.destSourceReg, op.baseReg, op.offsetReg);
		if (!op.halfWord && op.extend)
			return std::format("LDRSB R{}, [R{}, R{}]", op.destSourceReg, op.baseReg, op.offsetReg);
		return std::format("STRH R{}, [R{}, R{}]", op.destSourceReg, op.baseReg, op.offsetReg);
	}
};

#endif