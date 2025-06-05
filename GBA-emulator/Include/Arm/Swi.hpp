#ifndef Swi_H
#define Swi_H

#include <bit>
#include <cstdint>
#include <format>

#include "cplusplusRewrite/HwRegisters.h"

class Swi
{
public:
	struct SwiOpcode_thumb
	{
		uint16_t unused : 8;
		uint16_t reserved : 8;
	};

	static constexpr SwiOpcode_thumb fromOpcode_thumb(const uint16_t opcode)
	{
		return {
			.unused = static_cast<uint16_t>(opcode & 0xFF),             // bits 0–10
			.reserved = static_cast<uint16_t>((opcode >> 8) & 0xFF)    // bits 12–15
		};
	}

	static constexpr bool isThisOpcode_thumb(const uint16_t opcode)
	{
		const auto opcodeStruct = fromOpcode_thumb(opcode);
		return (opcodeStruct.reserved == 0b11011111);
	}

	static void execute(Registers& regs, const uint16_t opcode)
	{
		const uint32_t prev = regs.m_cpsr.val;
		regs.updateMode(ESUPER);
		regs[ELinkRegisterLR] = regs[EProgramCounter];
		regs[ESavedStatusRegister] = prev;
		//svc mode
		regs.m_cpsr.IRQDisable = 1;
		regs.m_cpsr.thumb = 0;
		regs.m_cpsr.mode = ESUPER;

		regs[TRegisters::EProgramCounter] = 0x8;
	}

	static auto disassemble(const uint16_t opcode)
	{
		return "SWI";
	}
};

#endif