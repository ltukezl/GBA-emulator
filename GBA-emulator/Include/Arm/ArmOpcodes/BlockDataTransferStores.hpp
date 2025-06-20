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
		
		// flags for ldm operation
		const bool baseInRList = op.rlist & (1 << op.baseReg);
		const bool pcInRlist = (op.rlist & (1 << EProgramCounter));
		const auto currentMode = regs.getMode();
		constexpr uint32_t offset = 4;

		// stm operational info
		uint32_t writebackAddress = regs[op.baseReg];
		writebackAddress += op.baseReg == 15 ? 4 : 0;

		// UB target addresses
		const uint32_t bits = std::popcount((op.rlist & ((1U << op.baseReg) - 1U)));
		const bool first = bits == 0;
		const uint32_t savedAddr = (bits << 2) + writebackAddress;
		
		if (c_op.loadPSR)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}

		for (size_t i = 0; i < 15; i++)
		{
			if (op.rlist & (1 << i))
			{
				writebackAddress += offset;
				if(i != op.baseReg)
					writeToAddress32(writebackAddress, regs[i]);
			}
		}

		if (pcInRlist)
		{
			writebackAddress += offset;
			writeToAddress32(writebackAddress, regs[EProgramCounter] + 8);
		}

		if (!c_op.writeback && baseInRList)
		{
			writeToAddress32(savedAddr + 4, regs[op.baseReg]);
		}
		else if (c_op.writeback && baseInRList && !first)
		{
			writeToAddress32(savedAddr + 4, writebackAddress);
		}
		else if (c_op.writeback && baseInRList && first)
		{
			writeToAddress32(savedAddr + 4, regs[op.baseReg]);
		}


		if constexpr (c_op.writeback)
			regs[op.baseReg] = writebackAddress;

		if (c_op.loadPSR)
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
		constexpr auto c_op = BlockDataTransfer::fromOpcode(iterOpcode);
		const auto op = BlockDataTransfer::fromOpcode(opcode);
		const bool baseInRList = op.rlist & (1 << op.baseReg);
		const bool pcInRlist = (op.rlist & (1 << EProgramCounter));
		uint32_t writebackAddress = regs[op.baseReg];
		writebackAddress += op.baseReg == 15 ? 4 : 0;
		const auto currentMode = regs.getMode();
		constexpr uint32_t offset = 4;
		uint32_t bits = std::popcount(op.rlist);
		uint32_t savedAddr = writebackAddress - (bits << 2);
		uint32_t bits2 = std::popcount((op.rlist & ((1U << op.baseReg) - 1U)));
		bool first = bits2 == 0;


		if (c_op.loadPSR)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}

		if (pcInRlist)
		{
			writebackAddress -= 4;
			writeToAddress32(writebackAddress, regs[EProgramCounter] + 8);
		}

		for (size_t i = 0; i < 15; i++)
		{
			if ((op.rlist << i) & 0x4000)
			{
				writebackAddress -= offset;
				if (op.baseReg != (14 - i))
					writeToAddress32(writebackAddress, regs[14 - i]);
				else if (c_op.writeback && (op.baseReg == (14 - i)) && !first)
					writeToAddress32(writebackAddress, savedAddr);
				else
					writeToAddress32(writebackAddress, regs[14 - i]);
			}
		}

		if constexpr (c_op.writeback)
			regs[op.baseReg] = writebackAddress;

		if (c_op.loadPSR)
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
		constexpr auto c_op = BlockDataTransfer::fromOpcode(iterOpcode);
		const auto op = BlockDataTransfer::fromOpcode(opcode);
		const bool baseInRList = op.rlist & (1 << op.baseReg);
		const bool pcInRlist = (op.rlist & (1 << EProgramCounter));
		uint32_t writebackAddress = regs[op.baseReg];
		writebackAddress += op.baseReg == 15 ? 4 : 0;
		const auto currentMode = regs.getMode();
		constexpr uint32_t offset = 4;
		uint32_t bits = std::popcount((op.rlist & ((1U << op.baseReg) - 1U)));
		bool first = bits == 0;
		uint32_t savedAddr = (bits << 2) + writebackAddress;


		if (c_op.loadPSR)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}

		for (size_t i = 0; i < 15; i++)
		{
			if (op.rlist & (1 << i))
			{
				if(i != op.baseReg)
					writeToAddress32(writebackAddress, regs[i]);
				writebackAddress += offset;
			}
		}

		if (pcInRlist)
		{
			writeToAddress32(writebackAddress, regs[EProgramCounter] + 8);
			writebackAddress += 4;
		}

		if (!c_op.writeback && baseInRList)
		{
			writeToAddress32(savedAddr, regs[op.baseReg]);
		}
		else if (c_op.writeback && baseInRList)
		{
			writeToAddress32(savedAddr, writebackAddress);
		}

		if constexpr (c_op.writeback)
			regs[op.baseReg] = writebackAddress;

		if (c_op.loadPSR)
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
		constexpr auto c_op = BlockDataTransfer::fromOpcode(iterOpcode);
		const auto op = BlockDataTransfer::fromOpcode(opcode);
		const bool baseInRList = op.rlist & (1 << op.baseReg);
		const bool pcInRlist = (op.rlist & (1 << EProgramCounter));
		uint32_t writebackAddress = regs[op.baseReg];
		writebackAddress += op.baseReg == 15 ? 4 : 0;
		const auto currentMode = regs.getMode();
		constexpr uint32_t offset = 4;
		uint32_t bits = std::popcount(op.rlist);
		uint32_t savedAddr = writebackAddress - (bits << 2);

		if (c_op.loadPSR)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}

		if (c_op.writeback && pcInRlist && op.baseReg == 15)
		{
			writeToAddress32(writebackAddress, savedAddr);
			writebackAddress -= offset;
		}
		else if (pcInRlist)
		{
			writeToAddress32(writebackAddress, regs[EProgramCounter] + 8);
			writebackAddress -= offset;
		}


		for (size_t i = 0; i < 15; i++)
		{
			if ((op.rlist << i) & 0x4000)
			{
				if (op.baseReg != (14 - i))
					writeToAddress32(writebackAddress, regs[14 - i]);
				else if (c_op.writeback && (op.baseReg == (14 - i)))
					writeToAddress32(writebackAddress, savedAddr);
				else
					writeToAddress32(writebackAddress, regs[op.baseReg]);

				writebackAddress -= offset;
			}
		}


		if constexpr (c_op.writeback)
			regs[op.baseReg] = writebackAddress;

		if (c_op.loadPSR)
		{
			regs.updateMode(currentMode);
		}
	}
};

#endif