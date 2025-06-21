#ifndef MRS_H
#define MRS_H

#include <bit>
#include <cstdint>
#include <format>

#include "CommonOperations/GbaStrings.hpp"
#include "cplusplusRewrite/HwRegisters.h"

class MRS
{
public:

	enum class PSR : uint32_t
	{
		CPSR,
		SPSR
	};

	struct MRSOpcode
	{
		uint32_t reserved1 : 12;
		uint32_t destination : 4;
		uint32_t reserved2 : 6;
		PSR source_PSR : 1;
		uint32_t reserved3 : 5;
		uint32_t condition : 4;
	};

	static constexpr uint16_t mask(const uint32_t opcode)
	{
		return opcode & (0x1 << 22);
	}

	static constexpr MRSOpcode fromOpcode(const uint32_t opcode)
	{
		return {
			.reserved1 = static_cast<uint32_t>(opcode & 0xFFF),
			.destination = static_cast<uint32_t>((opcode >> 12) & 0xF),
			.reserved2 = static_cast<uint32_t>((opcode >> 16) & 0x3F),
			.source_PSR = static_cast<PSR>((opcode >> 22) & 0x1),
			.reserved3 = static_cast<uint32_t>((opcode >> 23) & 0x1F),
			.condition = static_cast<uint32_t>((opcode >> 28) & 0xF),
		};
	}

	static constexpr bool isThisOpcode(const uint32_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved1 == 0) && (opcodeStruct.reserved2 == 0) && (opcodeStruct.reserved3 == 0b00010);
	}

	template<uint32_t iterOpcode>
	static void execute(Registers& regs, const uint32_t opcode)
	{
		constexpr auto c_op = fromOpcode(iterOpcode);
		const auto op = fromOpcode(opcode);

		if constexpr (c_op.source_PSR == PSR::CPSR)
		{
			regs[op.destination] = regs.m_cpsr.val;
		}
		else
		{
			regs[op.destination] = regs[ESavedStatusRegister];
		}
	}

	static auto disassemble(const uint32_t opcode)
	{
		const auto op = fromOpcode(opcode);
		const auto condition = condition_strings[op.condition];
		const auto source_reg = op.source_PSR == PSR::CPSR ? "CPSR" : "SPSR";
		return std::format("MRS{} R{}, {}", condition, op.destination, source_reg);
	}
};

#endif