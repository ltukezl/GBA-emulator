#pragma once

#include <cstdint>
#include <bit>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"
#include "cplusplusRewrite/MathOps.h"

class AddSubThumb
{
public:

	struct addSubRegisterOp {
		uint16_t destination : 3;
		uint16_t source : 3;
		uint16_t regOrImmediate : 3;
		uint16_t Sub : 1;
		uint16_t useImmediate : 1;
		uint16_t unused : 5;
	};

	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return opcode & (0x3 << 9);
	}

	static constexpr addSubRegisterOp fromOpcode(const uint16_t opcode)
	{
		return std::bit_cast<addSubRegisterOp>(opcode);
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.unused == 0b11);
	}

	template<uint32_t iterOpcode>
	static void execute(Registers& regs, const uint16_t opcode)
	{
		constexpr auto c_op = fromOpcode(iterOpcode);
		const auto op = fromOpcode(opcode);
		uint32_t value = 0;
		uint32_t reg_value = regs[op.source];
		if constexpr (c_op.useImmediate)
		{
			value = op.regOrImmediate;
		}
		else
		{
			value = regs[op.regOrImmediate];
		}

		if constexpr (c_op.Sub)
		{
			regs[op.destination] = mathOps::Sub::calculate(regs.m_cpsr, reg_value, value);
			mathOps::Sub::calcConditions(regs.m_cpsr, regs[op.destination], reg_value, value);
		}
		else
		{
			regs[op.destination] = mathOps::Add::calculate(regs.m_cpsr, reg_value, value);
			mathOps::Add::calcConditions(regs.m_cpsr, regs[op.destination], reg_value, value);
		}
	}

	static auto disassemble(const uint32_t opcode)
	{
		const auto op = fromOpcode(opcode);
		const auto operation = op.Sub ? "SUB" : "ADD";
		const auto prefix = op.useImmediate ? "#" : "R";
		return std::format("{} R{}, R{}, {}{:x}", operation, op.destination, op.source, prefix, op.regOrImmediate);
	}
};