#pragma once

#include <array>
#include <bit>
#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"
#include <cplusplusRewrite/MathOps.h>

class MovCmpAddSubImm
{
public:
	struct MovCmpAddSubImmOpcode
	{
		uint16_t offset : 8;
		uint16_t destination : 3;
		uint16_t instruction : 2;
		uint16_t unused : 3;
	};

	static constexpr std::array m_maths{ mathOps::Mov::calculate, mathOps::Cmp::calculate, mathOps::Add::calculate, mathOps::Sub::calculate };
	static constexpr std::array m_conditions{ mathOps::Mov::calcConditions, mathOps::Cmp::calcConditions, mathOps::Add::calcConditions, mathOps::Sub::calcConditions };
	static constexpr std::array math_strings = { "MOV", "CMP", "ADD", "SUB"};


	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return opcode & (0x3 << 11);
	}

	static constexpr MovCmpAddSubImmOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.offset = static_cast<uint16_t>(opcode & 0xFF),                    // bits 0–7
			.destination = static_cast<uint16_t>((opcode >> 8) & 0x7),         // bits 8–10
			.instruction = static_cast<uint16_t>((opcode >> 11) & 0x3),        // bits 11–12
			.unused = static_cast<uint16_t>((opcode >> 13) & 0x7)              // bits 13–15
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.unused == 1);
	}

	template<uint16_t iterOpcode>
	static void execute(Registers& regs, const uint16_t opcode)
	{
		constexpr auto c_op = fromOpcode(iterOpcode);
		const auto op = fromOpcode(opcode);

		constexpr auto func = m_maths[c_op.instruction];
		constexpr auto conditions = m_conditions[c_op.instruction];
		const auto regVal = regs[op.destination];
		const auto result = func(regs.m_cpsr, regVal, op.offset);
		conditions(regs.m_cpsr, result, regVal, op.offset);
		if constexpr (c_op.instruction != 1)
		{
			regs[op.destination] = result;
		}
	}

	static auto disassemble(const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		const auto operation = math_strings[op.instruction];
		const auto value = op.offset;
		return std::format("{} R{}, #0x{:x}", operation, op.destination, value);
	}
};