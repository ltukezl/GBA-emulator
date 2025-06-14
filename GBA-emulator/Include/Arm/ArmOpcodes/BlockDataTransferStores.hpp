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
		constexpr auto c_op = BlockDataTransfer::fromOpcode(iterOpcode);
		const auto op = BlockDataTransfer::fromOpcode(opcode);
		const bool baseInRList = op.rlist & (1 << op.baseReg);
		const bool pcInRlist = (op.rlist & (1 << EProgramCounter));
		uint32_t writebackAddress = regs[op.baseReg];
		const auto currentMode = regs.getMode();
		constexpr uint32_t offset = 4;
		uint32_t bits = std::popcount((op.rlist & ((1U << op.baseReg) - 1U)));
		bool first = bits == 0;
		uint32_t savedAddr = (bits << 2) + regs[op.baseReg];

		regs.updateMode(CpuModes_t::EUSR);

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
		else if (c_op.writeback && baseInRList)
		{
			writeToAddress32(savedAddr + 4, writebackAddress);
		}

		regs.updateMode(currentMode);

		if constexpr (c_op.writeback)
			regs[op.baseReg] = writebackAddress;
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
		const auto currentMode = regs.getMode();
		constexpr uint32_t offset = 4;


		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}

		if (pcInRlist)
		{
			writebackAddress -= 4;
			writeToAddress32(regs[op.baseReg], regs[EProgramCounter] + 8);
		}

		for (size_t i = 0; i < 15; i++)
		{
			if (op.rlist & (1 << i))
			{
				writebackAddress -= offset;
				writeToAddress32(writebackAddress, regs[i]);
			}
		}

		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(currentMode);
		}

		if constexpr (c_op.writeback)
			regs[op.baseReg] = writebackAddress;
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
		const auto currentMode = regs.getMode();
		constexpr uint32_t offset = 4;
		uint32_t bits = std::popcount((op.rlist & ((1U << op.baseReg) - 1U)));
		bool first = bits == 0;
		uint32_t savedAddr = (bits << 2) + regs[op.baseReg];


		if (c_op.loadPSR && !pcInRlist)
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

		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(currentMode);
		}

		if constexpr (c_op.writeback)
			regs[op.baseReg] = writebackAddress;
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
		const auto currentMode = regs.getMode();
		constexpr uint32_t offset = 4;


		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}

		if (pcInRlist)
		{
			writeToAddress32(regs[op.baseReg], regs[EProgramCounter] + 8);
			writebackAddress -= offset;
		}

		for (size_t i = 0; i < 15; i++)
		{
			if ((op.rlist << i) & 0x4000)
				//if (op.rlist & (1 << i))
			{
				writeToAddress32(writebackAddress, regs[14 - i]);
				writebackAddress -= offset;
			}
		}

		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(currentMode);
		}

		if constexpr (c_op.writeback)
			regs[op.baseReg] = writebackAddress;
	}
};

#endif