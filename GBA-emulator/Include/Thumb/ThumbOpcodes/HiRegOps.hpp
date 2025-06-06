#ifndef HighRegOps_H
#define HighRegOps_H

#include <cstdint>
#include <format>
#include <string>

#include "cplusplusRewrite/HwRegisters.h"
#include "cplusplusRewrite/MathOps.h"

class HighRegOps
{
public:
	struct HighRegOpsOpcode
	{
		uint16_t destination : 3;
		uint16_t source : 4;
		uint16_t destHiBit : 1;
		uint16_t instruction : 2;
		uint16_t reserved : 6;
	};

	static constexpr std::array mathOps_arr{ mathOps::Add::calculate, mathOps::Cmp::calculate, mathOps::Mov::calculate, mathOps::Mov::calculate };
	static constexpr std::array mathOps_arr_s { "ADD", "CMP", "MOV"};

	static consteval uint16_t mask(const uint16_t opcode)
	{
		return opcode & (7 << 7);
	}

	static constexpr HighRegOpsOpcode fromOpcode(const uint16_t opcode)
	{
		return {
			.destination = static_cast<uint16_t>(opcode & 0x0007),              // bits 0–2
			.source = static_cast<uint16_t>((opcode >> 3) & 0x000F),            // bits 3–6
			.destHiBit = static_cast<uint16_t>((opcode >> 7) & 0x0001),         // bit 7
			.instruction = static_cast<uint16_t>((opcode >> 8) & 0x0003),       // bits 8–9
			.reserved = static_cast<uint16_t>((opcode >> 10) & 0x003F)          // bits 10–15
		};
	}

	static consteval HighRegOpsOpcode fromOpcode_eval(const uint16_t opcode)
	{
		return {
			.destination = static_cast<uint16_t>(opcode & 0x0007),              // bits 0–2
			.source = static_cast<uint16_t>((opcode >> 3) & 0x000F),            // bits 3–6
			.destHiBit = static_cast<uint16_t>((opcode >> 7) & 0x0001),         // bit 7
			.instruction = static_cast<uint16_t>((opcode >> 8) & 0x0003),       // bits 8–9
			.reserved = static_cast<uint16_t>((opcode >> 10) & 0x003F)          // bits 10–15
		};
	}

	static constexpr bool isThisOpcode(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved == 0b010001);
	}

	template<uint16_t iterOpcode>
	static void execute(Registers& regs, const uint16_t opcode)
	{
		constexpr auto c_op = fromOpcode_eval(iterOpcode);
		const auto op = fromOpcode(opcode);
		uint8_t newDestinationReg = op.destination;
		uint32_t operand2 = regs[op.source];

		if constexpr (c_op.destHiBit)
		{
			newDestinationReg += 8;
		}

		uint32_t operand1 = regs[newDestinationReg];

		if (op.source == EProgramCounter)
		{
			operand2 += 2;
			operand2 &= ~1;
		}

		if (newDestinationReg == EProgramCounter)
		{
			operand1 += 2;
			operand2 &= ~1;
		}

		if constexpr (c_op.instruction == 3)
		{
			regs[EProgramCounter] = operand2 & ~1;
			regs.m_cpsr.thumb = operand2 & 1;
			return;
		}

		constexpr auto mathFunc = mathOps_arr[c_op.instruction];
		const auto result = mathFunc(regs.m_cpsr, operand1, operand2);
		if constexpr (c_op.instruction == 1)
		{
			mathOps::Cmp::calcConditions(regs.m_cpsr, result, operand1, operand2);
			return;
		}

		regs[newDestinationReg] = result;
	}

	static std::string decodeRegisterName(uint16_t reg)
	{
		if (reg == EStackPointer)
		{
			return "SP";
		}
		if (reg == EProgramCounter)
		{
			return "PC";
		}
		if (reg == ELinkRegisterLR)
		{
			return "LR";
		}
		return std::format("R{}", reg);
	}

	static auto disassemble(const uint16_t opcode)
	{
		const auto op = fromOpcode(opcode);
		uint8_t newDestinationReg = op.destination;

		if (op.destHiBit)
		{
			newDestinationReg += 8;
		}

		const auto reg2 = decodeRegisterName(op.source);
		if (op.instruction == 3)
		{
			return std::format("BX {}", reg2);
		}
		auto reg1 = decodeRegisterName(newDestinationReg);

		return std::format("{} {}, {}", mathOps_arr_s[op.instruction], reg1, reg2);
	}
};

#endif