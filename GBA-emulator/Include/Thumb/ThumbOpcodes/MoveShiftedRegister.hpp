#pragma once

#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"
#include "cplusplusRewrite/Shifts.h"

class MoveShiftedRegister
{
public:

	static constexpr std::array m_shifts{ shifts::Lsl::shift, shifts::Lsr::shift, shifts::Asr::shift };
	static constexpr std::array m_conditions{ shifts::Lsl::calcConditions, shifts::Lsr::calcConditions, shifts::Asr::calcConditions };
	static constexpr std::array shift_strings = { "LSL", "LSR", "ASR" };

	struct MoveShiftedRegisterOpcode {
		uint16_t destination : 3;
		uint16_t source : 3;
		uint16_t immediate : 5;
		uint16_t instruction : 2;
		uint16_t unused : 3;
	};

	static constexpr uint16_t mask(const uint16_t opcode)
	{
		return opcode & (0x3 << 11);
	}

	static constexpr MoveShiftedRegisterOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.destination = static_cast<uint16_t>(opcode & 0b111),             // bits 0–2
			.source = static_cast<uint16_t>((opcode >> 3) & 0b111),           // bits 3–5
			.immediate = static_cast<uint16_t>((opcode >> 6) & 0b11111),        // bits 6–10
			.instruction = static_cast<uint16_t>((opcode >> 11) & 0b11),     // bits 11–12
			.unused = static_cast<uint16_t>((opcode >> 13) & 0b111)           // bits 13–15
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.unused == 0);
	}

	template<uint16_t iterOpcode>
	static void execute(Registers& regs, const uint16_t opcode)
	{
		constexpr auto c_op = fromOpcode(iterOpcode);
		static_assert(c_op.instruction != 3);
		const auto op = fromOpcode(opcode);
		constexpr auto func = m_shifts[c_op.instruction];
		constexpr auto func_conditions = m_conditions[c_op.instruction];
		const auto reg_val = regs[op.source];
		auto imm = op.immediate;
		if (op.immediate == 0 && (op.instruction == 1 || op.instruction == 2))
			imm = 32;
		const auto result = func(reg_val, imm);
		func_conditions(regs.m_cpsr, result, reg_val, imm);
		regs[op.destination] = result;
	}

	static auto disassemble(const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		const auto operation = shift_strings[op.instruction];
		const auto value = op.immediate;
		return std::format("{} R{}, R{}, #0x{:x}", operation, op.destination, op.source, value);
	}
};