#ifndef BLOCKDATATRANSFERLOADS_J
#define BLOCKDATATRANSFERLOADS_J

#include <cstdint>

#include "Arm/ArmOpcodes/BlockDataTransfer.hpp"
#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

class BlockDataTransferLoad
{
public:
	static constexpr bool isThisOpcode(const uint32_t opcode)
	{
		const auto opcodeStruct = BlockDataTransfer::fromOpcode(opcode);
		return (opcodeStruct.loadStore == 1);
	}

	template<uint32_t iterOpcode>
	static void execute(Registers& regs, const uint32_t opcode)
	{
		// opcode preample
		constexpr auto c_op = BlockDataTransfer::fromOpcode(iterOpcode);
		const auto op = BlockDataTransfer::fromOpcode(opcode);

		// bug of empty rlist
		if (op.rlist == 0)
		{
			BlockDataTransfer::empty_rlist_bug_ldm(regs, op);
			return;
		}

		// flags for ldm operation
		const bool pcInRlist = (op.rlist & (1 << EProgramCounter));
		const auto currentMode = regs.getMode();
		constexpr uint32_t offset = 4;

		// ldm operational data
		const uint32_t amount_of_transactions = std::popcount(op.rlist);
		uint32_t internal_base_address = regs[op.baseReg];
		internal_base_address += op.baseReg == EProgramCounter ? 4 : 0;
		const uint32_t final_writeback_address = c_op.addOffset ? internal_base_address + (amount_of_transactions << 2) : internal_base_address - (amount_of_transactions << 2);

		// internally decrementing ldm is actually always increasing, but arm calculates the new base address.
		if constexpr (c_op.addOffset == 0)
		{
			internal_base_address -= (amount_of_transactions << 2);
		}
		
		// micro optimization to reduce amount of redundant loops in transfer
		const size_t rlist_first_reg = std::countr_zero(op.rlist);

		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}
		else if (c_op.loadPSR && pcInRlist)
		{
			regs.m_cpsr.val = regs[ESavedStatusRegister];
		}

		if constexpr (c_op.writeback)
			regs[op.baseReg] = final_writeback_address;

		if constexpr ((c_op.preIndex ^ c_op.addOffset) == 1)
			internal_base_address -= offset;

		for (size_t i = rlist_first_reg; i < 16; i++)
		{
			if (op.rlist & (1 << i))
			{
				internal_base_address += offset;
				regs[i] = loadFromAddress32(internal_base_address, false);
			}
		}

		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(currentMode);
		}
	}
};

#endif