#ifndef BlockDataTransfer_H
#define BlockDataTransfer_H

#include <bit>
#include <cstdint>
#include <format>

#include "CommonOperations/GbaStrings.hpp"
#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

class BlockDataTransfer
{
public:
	struct BlockDataTransferOpcode
	{
		uint32_t rlist : 16;
		uint32_t baseReg : 4;
		uint32_t loadStore : 1;
		uint32_t writeback : 1;
		uint32_t loadPSR : 1;
		uint32_t addOffset : 1;
		uint32_t preIndex : 1;
		uint32_t reserved : 3;
		uint32_t condition : 4;
	};

	static constexpr uint32_t mask(const uint32_t opcode)
	{
		return opcode & (0x1F << 20);
	}

	static constexpr BlockDataTransferOpcode fromOpcode(const uint32_t opcode)
	{
		return {
			.rlist = static_cast<uint32_t>(opcode & 0xFFFF),             // bits 0–15
			.baseReg = static_cast<uint32_t>((opcode >> 16) & 0xF),        // bits 16–19
			.loadStore = static_cast<uint32_t>((opcode >> 20) & 0x1),        // bit 20
			.writeback = static_cast<uint32_t>((opcode >> 21) & 0x1),        // bit 21
			.loadPSR = static_cast<uint32_t>((opcode >> 22) & 0x1),        // bit 22
			.addOffset = static_cast<uint32_t>((opcode >> 23) & 0x1),        // bit 23
			.preIndex = static_cast<uint32_t>((opcode >> 24) & 0x1),        // bit 24
			.reserved = static_cast<uint32_t>((opcode >> 25) & 0x7),        // bits 25–27
			.condition = static_cast<uint32_t>((opcode >> 28) & 0xF)         // bits 28–31
		};
	}

	static constexpr bool isThisOpcode(const uint32_t opcode)
	{
		const auto opcodeStruct = fromOpcode(opcode);
		return (opcodeStruct.reserved == 0b100);
	}

	static inline void empty_rlist_bug_ldm(Registers& regs, const BlockDataTransferOpcode op)
	{
		regs[EProgramCounter] = loadFromAddress32(regs[op.baseReg]);
		regs[op.baseReg] += 0x40;
		return;
	}

	static inline void empty_rlist_bug_stm(Registers& regs, const BlockDataTransferOpcode op)
	{
		regs[op.baseReg] -= 0x40;
		writeToAddress32(regs[op.baseReg], regs[EProgramCounter] + 8);
		return;
	}

	static auto disassemble(const uint32_t opcode)
	{
		const auto op = fromOpcode(opcode);
		const auto opcodeName = op.loadStore ? "LDM" : "STM";
		const auto increment = op.addOffset ? 'I' : 'D'; // Increment / Decrement
		const auto preIncrement = op.preIndex ? 'B' : 'A'; // Before / After
		const auto condition = condition_strings[op.condition];
		const auto baseReg = op.baseReg;
		const auto writeback = (op.writeback) ? "!" : "";
		const auto rlist = create_rlist(op.rlist, 16);
		const auto load_psr = op.loadPSR ? "^" : "";
		return std::format("{}{}{}{} R{}{}, {{{}}}{}",opcodeName, increment, preIncrement, condition, baseReg, writeback, rlist, load_psr);
	}

	template<BlockDataTransferOpcode op>
	static consteval auto memLoadOp()
	{
		if constexpr (op.loadStore)
		{
			return &loadFromAddress32;
		}
		else
		{
			return &loadFromAddress;
		}
	}

	template<BlockDataTransferOpcode op>
	static consteval auto memStoreOp()
	{
		if constexpr (op.loadStore)
		{
			return &writeToAddress32;
		}
		else
		{
			return  &writeToAddress;
		}
	}
};

#endif