#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"
#include "cplusplusRewrite/MathOps.h"
#include "cplusplusRewrite/Shifts.h"

class AluOps
{
public:

	static constexpr std::array conditionFunctions = {
		mathOps::And::calcConditions,
		mathOps::Eor::calcConditions,
		shifts::Lsl::calcConditions,
		shifts::Lsr::calcConditions,
		shifts::Asr::calcConditions,
		mathOps::Adc::calcConditions,
		mathOps::Sbc::calcConditions,
		shifts::Ror::calcConditions,
		mathOps::Tst::calcConditions,
		mathOps::Neg::calcConditions,
		mathOps::Cmp::calcConditions,
		mathOps::Cmn::calcConditions,
		mathOps::Orr::calcConditions,
		mathOps::Mul::calcConditions,
		mathOps::Bic::calcConditions,
		mathOps::Mvn::calcConditions
	};

	static constexpr std::array math_strings = { "AND", "EOR", "LSL", "LSR", "ASR", "ADC", "SBC", "ROR", "TST", "NEG", "CMP", "CMN", "ORR", "MUL", "BIC", "MVN" };

	struct AluOpsOpcode
	{
		uint16_t destination : 3;
		uint16_t source : 3;
		uint16_t instruction : 4;
		uint16_t unused : 6;
	};

	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return opcode & (0xF << 6);
	}

	static constexpr AluOpsOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.destination = static_cast<uint16_t>(opcode & 0b111), 
			.source = static_cast<uint16_t>((opcode >> 3) & 0b111),
			.instruction = static_cast<uint16_t>((opcode >> 6) & 0b1111),
			.unused = static_cast<uint16_t>((opcode >> 10) & 0x3f)
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.unused == 0b1'0000);
	}

	template<uint16_t iterOpcode>
	static void execute(Registers& regs, const uint16_t opcode)
	{
		constexpr auto c_op = fromOpcode(iterOpcode);
		const auto op = fromOpcode(opcode);

		uint32_t result = 0;
		const uint32_t operand1 = regs[op.destination];
		const uint32_t operand2 = regs[op.source];

		uint32_t& dest = regs[op.destination];

		switch (c_op.instruction)
		{
			case 0: result = mathOps::And::calculate(regs.m_cpsr, operand1, operand2); dest = result; break;
			case 1: result = mathOps::Eor::calculate(regs.m_cpsr, operand1, operand2); dest = result; break;
			case 2: result = shifts::Lsl::shift(operand1, operand2); dest = result; break;
			case 3: result = shifts::Lsr::shift(operand1, operand2); dest = result; break;
			case 4: result = shifts::Asr::shift(operand1, operand2); dest = result; break;
			case 5: result = mathOps::Adc::calculate(regs.m_cpsr, operand1, operand2); dest = result; break;
			case 6: result = mathOps::Sbc::calculate(regs.m_cpsr, operand1, operand2); dest = result; break;
			case 7: result = shifts::Ror::shift(operand1, operand2); dest = result; break;
			case 8: result = mathOps::Tst::calculate(regs.m_cpsr, operand1, operand2); break; // no writeback
			case 9: result = mathOps::Neg::calculate(regs.m_cpsr, 0, operand2); dest = result; break;
			case 10: result = mathOps::Cmp::calculate(regs.m_cpsr, operand1, operand2); break;
			case 11: result = mathOps::Cmn::calculate(regs.m_cpsr, operand1, operand2); break;
			case 12: result = mathOps::Orr::calculate(regs.m_cpsr, operand1, operand2); dest = result; break;
			case 13: result = mathOps::Mul::calculate(regs.m_cpsr, operand1, operand2); dest = result; break;
			case 14: result = mathOps::Bic::calculate(regs.m_cpsr, operand1, operand2); dest = result; break;
			case 15: result = mathOps::Mvn::calculate(regs.m_cpsr, operand1, operand2); dest = result; break;
			default: assert(false); // handle invalid instruction
		}

		conditionFunctions[c_op.instruction](regs.m_cpsr, result, operand1, operand2);
	}

	static auto disassemble(const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		const auto operation = math_strings[op.instruction];
		return std::format("{} R{}, R{}", operation, op.destination, op.source);
	}
};