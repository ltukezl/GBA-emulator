#pragma once
#ifndef BLOCKDATATRANSFERSTORES_H
#define BLOCKDATATRANSFERSTORES_H

#include <bit>
#include <cstdint>

#include "Arm/ArmOpcodes/BlockDataTransfer.hpp"
#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

class BlockDataTransferPreStoreAdd
{
public:
	static constexpr bool isThisOpcode(const uint32_t opcode)
	{
		const auto opcodeStruct = BlockDataTransfer::fromOpcode(opcode);
		return (opcodeStruct.preIndex == 1) && (opcodeStruct.loadStore == 0) && (opcodeStruct.addOffset == 1);
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
		// to simplify as pipeline is not accessable 
		regs[EProgramCounter] += 4;
		const uint32_t oldBase = regs[op.baseReg];
		uint32_t writebackAddress = oldBase;
		// pipeline is again more ahead in here 
		regs[EProgramCounter] += 4;

		// UB target addresses
		const uint32_t bits = std::popcount(op.rlist);
		const bool first = std::countr_zero(op.rlist) == op.baseReg;
		const uint32_t savedAddr = (bits << 2) + writebackAddress;

		if constexpr (c_op.loadPSR)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}

		for (size_t i = 0; i < 16; i++)
		{
			if (op.rlist & (1 << i))
			{
				writebackAddress += offset;
				if (c_op.writeback && (i == op.baseReg) && !first)
					writeToAddress32(writebackAddress, savedAddr);
				else if (c_op.writeback && (i == op.baseReg) && first)
					writeToAddress32(writebackAddress, oldBase);
				else
					writeToAddress32(writebackAddress, regs[i]);
			}
		}

		// re adjust ppc back in here
		regs[EProgramCounter] -= 8;

		if (c_op.writeback)
			regs[op.baseReg] = writebackAddress;

		if constexpr (c_op.loadPSR)
		{
			regs.updateMode(currentMode);
		}
	}
};

class BlockDataTransferPreStoreSub
{
public:
	static constexpr bool isThisOpcode(const uint32_t opcode)
	{
		const auto opcodeStruct = BlockDataTransfer::fromOpcode(opcode);
		return (opcodeStruct.preIndex == 1) && (opcodeStruct.loadStore == 0) && (opcodeStruct.addOffset == 0);
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
		// to simplify as pipeline is not accessable 
		regs[EProgramCounter] += 4;
		const uint32_t oldBase = regs[op.baseReg];
		uint32_t writebackAddress = oldBase;
		// pipeline is again more ahead in here 
		regs[EProgramCounter] += 4;

		// UB target addresses
		const bool first = std::countr_zero(op.rlist) == op.baseReg;

		const uint32_t transfers = std::popcount(op.rlist);
		writebackAddress -= (transfers << 2);
		const uint32_t dec_writeback = writebackAddress;

		if (c_op.loadPSR)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}

		for (size_t i = 0; i < 16; i++)
		{
			if (op.rlist & (1 << i))
			{
				if (c_op.writeback && (i == op.baseReg) && !first)
					writeToAddress32(writebackAddress, dec_writeback);
				else if (c_op.writeback && (i == op.baseReg) && first)
					writeToAddress32(writebackAddress, oldBase);
				else
					writeToAddress32(writebackAddress, regs[i]);
				writebackAddress += offset;
			}
		}

		// re adjust ppc back in here
		regs[EProgramCounter] -= 8;

		if (c_op.writeback)
			regs[op.baseReg] = dec_writeback;

		if constexpr (c_op.loadPSR)
		{
			regs.updateMode(currentMode);
		}
	}
};

class BlockDataTransferPostStoreAdd
{
public:
	static constexpr bool isThisOpcode(const uint32_t opcode)
	{
		const auto opcodeStruct = BlockDataTransfer::fromOpcode(opcode);
		return (opcodeStruct.preIndex == 0) && (opcodeStruct.loadStore == 0) && (opcodeStruct.addOffset == 1);
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
		// to simplify as pipeline is not accessable 
		regs[EProgramCounter] += 4;
		const uint32_t oldBase = regs[op.baseReg];
		uint32_t writebackAddress = oldBase;
		// pipeline is again more ahead in here 
		regs[EProgramCounter] += 4;

		// UB target addresses
		const uint32_t bits = std::popcount(op.rlist);
		const bool first = std::countr_zero(op.rlist) == op.baseReg;
		const uint32_t savedAddr = (bits << 2) + writebackAddress;

		if constexpr (c_op.loadPSR)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}

		for (size_t i = 0; i < 16; i++)
		{
			if (op.rlist & (1 << i))
			{
				if (c_op.writeback && (i == op.baseReg) && !first)
					writeToAddress32(writebackAddress, savedAddr);
				else if (c_op.writeback && (i == op.baseReg) && first)
					writeToAddress32(writebackAddress, oldBase);
				else
					writeToAddress32(writebackAddress, regs[i]);
				writebackAddress += offset;
			}
		}

		// re adjust ppc back in here
		regs[EProgramCounter] -= 8;

		if (c_op.writeback)
			regs[op.baseReg] = writebackAddress;

		if constexpr (c_op.loadPSR)
		{
			regs.updateMode(currentMode);
		}
	}
};

class BlockDataTransferPostStoreSub
{
public:
	static constexpr bool isThisOpcode(const uint32_t opcode)
	{
		const auto opcodeStruct = BlockDataTransfer::fromOpcode(opcode);
		return (opcodeStruct.preIndex == 0) && (opcodeStruct.loadStore == 0) && (opcodeStruct.addOffset == 0);
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
		// to simplify as pipeline is not accessable 
		regs[EProgramCounter] += 4;
		const uint32_t oldBase = regs[op.baseReg];
		uint32_t writebackAddress = oldBase;
		// pipeline is again more ahead in here 
		regs[EProgramCounter] += 4;

		// UB target addresses
		const bool first = std::countr_zero(op.rlist) == op.baseReg;

		const uint32_t transfers = std::popcount(op.rlist);
		writebackAddress -= (transfers << 2);
		const uint32_t dec_writeback = writebackAddress;

		if (c_op.loadPSR)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}

		for (size_t i = 0; i < 16; i++)
		{
			if (op.rlist & (1 << i))
			{
				writebackAddress += offset;
				if (c_op.writeback && (i == op.baseReg) && !first)
					writeToAddress32(writebackAddress, dec_writeback);
				else if (c_op.writeback && (i == op.baseReg) && first)
					writeToAddress32(writebackAddress, oldBase);
				else
					writeToAddress32(writebackAddress, regs[i]);
			}
		}

		// re adjust ppc back in here
		regs[EProgramCounter] -= 8;

		if (c_op.writeback)
			regs[op.baseReg] = dec_writeback;

		if constexpr (c_op.loadPSR)
		{
			regs.updateMode(currentMode);
		}
	}
};

#endif