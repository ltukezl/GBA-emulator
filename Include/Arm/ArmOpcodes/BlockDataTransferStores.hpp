#pragma once
#ifndef BLOCKDATATRANSFERSTORES_H
#define BLOCKDATATRANSFERSTORES_H

#include <bit>
#include <cstdint>

#include "Arm/ArmOpcodes/BlockDataTransfer.hpp"
#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

class BlockDataTransferStore
{
public:
	static constexpr bool isThisOpcode(const uint32_t opcode)
	{
		const auto opcodeStruct = BlockDataTransfer::fromOpcode(opcode);
		return (opcodeStruct.loadStore == 0);
	}

	template<uint32_t iterOpcode>
	static void execute(Registers& regs, const uint32_t opcode)
	{
		//op code preample
		constexpr auto c_op = BlockDataTransfer::fromOpcode(iterOpcode);
		const auto op = BlockDataTransfer::fromOpcode(opcode);

		// bug of empty rlist
		if (op.rlist == 0)
		{
			BlockDataTransfer::empty_rlist_bug_stm(regs, op);
			return;
		}

		// flags for stm operation
		const auto currentMode = regs.getMode();
		constexpr uint32_t offset = 4;

		// stm operational info

		uint32_t writebackAddress = regs[op.baseReg];
		writebackAddress += op.baseReg == EProgramCounter ? 4 : 0;

		// UB target addresses
		const size_t rlist_first_reg = std::countr_zero(op.rlist);

		const uint32_t transfers = std::popcount(op.rlist);
		const uint32_t savedAddr = c_op.addOffset ? writebackAddress + (transfers << 2) : writebackAddress - (transfers << 2);
		if constexpr (c_op.addOffset == 0)
		{
			writebackAddress -= (transfers << 2);
		}

		if constexpr ((c_op.preIndex ^ c_op.addOffset) == 1)
			writebackAddress -= offset;

		if constexpr (c_op.loadPSR)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}

		// during first transfer the final address hasn't been calculated yet.
		writebackAddress += offset;
		writeToAddress32(writebackAddress, regs[rlist_first_reg]);

		// re adjust ppc back in here
		regs[EProgramCounter] += 8;

		if constexpr (c_op.writeback)
			regs[op.baseReg] = savedAddr;

		for (size_t i = rlist_first_reg + 1; i < 16; i++)
		{
			if (op.rlist & (1 << i))
			{
				writebackAddress += offset;
				writeToAddress32(writebackAddress, regs[i]);
			}
		}

		if (c_op.writeback && op.baseReg == EProgramCounter)
			regs[op.baseReg] += 8;

		regs[EProgramCounter] -= 8;

		if constexpr (c_op.loadPSR)
		{
			regs.updateMode(currentMode);
		}
	}
};

#endif