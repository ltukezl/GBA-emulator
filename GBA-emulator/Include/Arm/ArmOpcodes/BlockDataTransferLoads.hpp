#ifndef BLOCKDATATRANSFERLOADS_J
#define BLOCKDATATRANSFERLOADS_J

#include "Arm/ArmOpcodes/BlockDataTransfer.hpp"

class BlockDataTrasnferPreLoad
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
		constexpr auto c_op = BlockDataTransfer::fromOpcode(iterOpcode);
		const auto op = BlockDataTransfer::fromOpcode(opcode);
		const bool baseInRList = op.rlist & (1 << op.baseReg);
		const bool pcInRlist = (op.rlist & (1 << EProgramCounter));
		uint32_t writebackAddress = regs[op.baseReg] & ~0x3;
		const auto currentMode = regs.getMode();
		constexpr uint32_t offset = 4;

		if (op.rlist == 0)
		{
			BlockDataTransfer::empty_rlist_bug(regs, op);
			return;
		}

		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(CpuModes_t::EUSR);
		}

		for (size_t i = 0; i < 16; i++)
		{
			if (op.rlist & (1 << i))
			{
				writebackAddress += offset;
				regs[i] = loadFromAddress32(writebackAddress, false);
				if (i == 15)
					regs[ESavedStatusRegister] = regs.m_cpsr.val;
			}
		}

		if (c_op.loadPSR && !pcInRlist)
		{
			regs.updateMode(currentMode);
		}

		if (c_op.writeback && !baseInRList)
			regs[op.baseReg] = writebackAddress;
			
	}
};

#endif