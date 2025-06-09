#pragma once

#ifndef MRS_H
#define MRS_H

#include <bit>
#include <cstdint>
#include <format>

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

	static constexpr auto fromOpcode(const uint32_t opcode)
	{
		return std::bit_cast<MRSOpcode>(opcode);
	}

	static constexpr bool isThisOpcode(const uint32_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved == 9);
	}

	template<uint32_t iterOpcode>
	static void execute(Registers& regs, const uint32_t opcode)
	{
		constexpr auto c_op = fromOpcode(iterOpcode);
		const auto op = fromOpcode(opcode);
	}

	static auto disassemble(const uint32_t opcode)
	{

	}
};

#endif