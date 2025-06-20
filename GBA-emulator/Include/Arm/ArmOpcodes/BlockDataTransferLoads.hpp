#ifndef BLOCKDATATRANSFERLOADS_J
#define BLOCKDATATRANSFERLOADS_J

#include <cstdint>

#include "Arm/ArmOpcodes/BlockDataTransfer.hpp"
#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

class BlockDataTransferPreLoadAdd
{
public:
	static constexpr bool isThisOpcode(const uint32_t opcode)
	{
		const auto opcodeStruct = BlockDataTransfer::fromOpcode(opcode);
		return (opcodeStruct.preIndex == 1) && (opcodeStruct.loadStore == 1) && (opcodeStruct.addOffset == 1);
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
			BlockDataTransfer::empty_rlist_bug(regs, op);
			return;
		}

		// flags for ldm operation
		const bool baseInRList = op.rlist & (1 << op.baseReg);
		const bool pcInRlist = (op.rlist & (1 << EProgramCounter));
		const auto currentMode = regs.getMode();
		constexpr uint32_t offset = 4;

		// ldm operational data
		uint32_t writebackAddress = regs[op.baseReg];
		writebackAddress += op.baseReg == 15 ? 4 : 0;

		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}
		else if (c_op.loadPSR && pcInRlist)
		{
			regs.m_cpsr.val = regs[ESavedStatusRegister];
		}

		for (size_t i = 0; i < 16; i++)
		{
			if (op.rlist & (1 << i))
			{
				writebackAddress += offset;
				regs[i] = loadFromAddress32(writebackAddress, false);
			}
		}

		if (c_op.writeback && !baseInRList)
			regs[op.baseReg] = writebackAddress;

		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(currentMode);
		}
	}
};

class BlockDataTransferPostLoadSub
{
public:
	static constexpr bool isThisOpcode(const uint32_t opcode)
	{
		const auto opcodeStruct = BlockDataTransfer::fromOpcode(opcode);
		return (opcodeStruct.preIndex == 0) && (opcodeStruct.loadStore == 1) && (opcodeStruct.addOffset == 0);
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
			BlockDataTransfer::empty_rlist_bug(regs, op);
			return;
		}

		// flags for ldm operation
		const bool baseInRList = op.rlist & (1 << op.baseReg);
		const bool pcInRlist = (op.rlist & (1 << EProgramCounter));
		const auto currentMode = regs.getMode();
		constexpr uint32_t offset = 4;

		// ldm operational data
		uint32_t writebackAddress = regs[op.baseReg];
		writebackAddress += op.baseReg == 15 ? 4 : 0;

		const uint32_t transfers = std::popcount(op.rlist);
		writebackAddress -= (transfers << 2);

		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}
		else if (c_op.loadPSR && pcInRlist)
		{
			regs.m_cpsr.val = regs[ESavedStatusRegister];
		}

		if (c_op.writeback && !baseInRList)
			regs[op.baseReg] = writebackAddress;

		for (size_t i = 0; i < 16; i++)
		{
			if (op.rlist & (1 << i))
			{
				writebackAddress += offset;
				regs[i] = loadFromAddress32(writebackAddress, false);
			}
		}

		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(currentMode);
		}
	}
};

class BlockDataTransferPreLoadSub
{
public:
	static constexpr bool isThisOpcode(const uint32_t opcode)
	{
		const auto opcodeStruct = BlockDataTransfer::fromOpcode(opcode);
		return (opcodeStruct.preIndex == 1) && (opcodeStruct.loadStore == 1) && (opcodeStruct.addOffset == 0);
	}

	template<uint32_t iterOpcode>
	static void execute(Registers& regs, const uint32_t opcode)
	{
		// opcode preample
		constexpr auto c_op = BlockDataTransfer::fromOpcode(iterOpcode);
		const auto op = BlockDataTransfer::fromOpcode(opcode);

		// flags for ldm operation
		const bool baseInRList = op.rlist & (1 << op.baseReg);
		const bool pcInRlist = (op.rlist & (1 << EProgramCounter));
		const auto currentMode = regs.getMode();
		constexpr uint32_t offset = 4;

		// ldm operational data
		uint32_t writebackAddress = regs[op.baseReg];
		writebackAddress += op.baseReg == 15 ? 4 : 0;

		const uint32_t transfers = std::popcount(op.rlist);
		writebackAddress -= (transfers << 2);
		const uint32_t dec_writeback = writebackAddress;

		// bug of empty rlist
		if (op.rlist == 0)
		{
			BlockDataTransfer::empty_rlist_bug(regs, op);
			return;
		}

		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}
		else if (c_op.loadPSR && pcInRlist)
		{
			regs.m_cpsr.val = regs[ESavedStatusRegister];
		}

		for (size_t i = 0; i < 16; i++)
		{
			if (op.rlist & (1 << i))
			{
				regs[i] = loadFromAddress32(writebackAddress, false);
				writebackAddress += offset;
			}
		}

		if (c_op.writeback && !baseInRList)
			regs[op.baseReg] = dec_writeback;

		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(currentMode);
		}
	}
};

class BlockDataTransferPostLoadAdd
{
public:
	static constexpr bool isThisOpcode(const uint32_t opcode)
	{
		const auto opcodeStruct = BlockDataTransfer::fromOpcode(opcode);
		return (opcodeStruct.preIndex == 0) && (opcodeStruct.loadStore == 1) && (opcodeStruct.addOffset == 1);
	}

	template<uint32_t iterOpcode>
	static void execute(Registers& regs, const uint32_t opcode)
	{
		// opcode preample
		constexpr auto c_op = BlockDataTransfer::fromOpcode(iterOpcode);
		const auto op = BlockDataTransfer::fromOpcode(opcode);

		// flags for ldm operation
		const bool baseInRList = op.rlist & (1 << op.baseReg);
		const bool pcInRlist = (op.rlist & (1 << EProgramCounter));
		const auto currentMode = regs.getMode();
		constexpr uint32_t offset = 4;

		// ldm operational data
		uint32_t writebackAddress = regs[op.baseReg];
		writebackAddress += op.baseReg == 15 ? 4 : 0;

		// bug of empty rlist
		if (op.rlist == 0)
		{
			BlockDataTransfer::empty_rlist_bug(regs, op);
			return;
		}

		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}
		else if (c_op.loadPSR && pcInRlist)
		{
			regs.m_cpsr.val = regs[ESavedStatusRegister];
		}

		for (size_t i = 0; i < 16; i++)
		{
			if (op.rlist & (1 << i))
			{
				regs[i] = loadFromAddress32(writebackAddress, false);
				writebackAddress += offset;
			}
		}

		if (c_op.writeback && !baseInRList)
			regs[op.baseReg] = writebackAddress;

		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(currentMode);
		}
	}
};

#endif