#pragma once

#include "Constants.h"
#include <array>
#include <bit>
#include <cstdint>

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
		return std::bit_cast<MovCmpAddSubImmOpcode>(opcode);
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
		return std::format("{} R{}, #{:x}", operation, op.destination, value);
	}
};