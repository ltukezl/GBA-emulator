#pragma once
#include <bit>
#include <cstdint>
#include <cassert>

#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"


namespace SingleDataTransfer {

	enum class loadStore_t : uint32_t
	{
		EStore,
		ELoad,
	};

	enum class writeBack_t : uint32_t
	{
		ENoWriteback,
		EWriteback
	};

	enum class byteWord_t : uint32_t
	{
		EWord,
		EByte
	};

	enum class upDown_t : uint32_t
	{
		ESubstract,
		EAdd
	};

	enum class prePost_t : uint32_t
	{
		EPre,
		EPost,
	};

	enum class immediate_t : uint32_t
	{
		EImmediate,
		EBarrelShifter,
	};

	struct SingleDataTransfer_t {
		uint32_t offset : 12; //barrel shifter
		uint32_t destinationRegister : 4;
		uint32_t baseRegister : 4;
		loadStore_t loadBit : 1;
		writeBack_t writeBack : 1;
		byteWord_t byteTransfer : 1;
		upDown_t addOffset : 1;
		prePost_t preIndexing : 1;
		immediate_t immediateOffset : 1;
		uint32_t unused : 2;
		uint32_t executionCondition : 4;
	};

	constexpr uint32_t fromFields(const uint32_t offset, const uint32_t destinationRegister, const uint32_t baseRegister, const loadStore_t loadBit, const writeBack_t writeBack, 
		                          const byteWord_t byteTransfer, const upDown_t addOffset, const prePost_t preIndexing, const immediate_t immediateOffset) {
		SingleDataTransfer_t opcode {};
		opcode.offset = offset;
		opcode.destinationRegister = destinationRegister;
		opcode.baseRegister = baseRegister;
		opcode.loadBit = loadBit;
		opcode.writeBack = writeBack;
		opcode.byteTransfer = byteTransfer;
		opcode.addOffset = addOffset;
		opcode.preIndexing = preIndexing;
		opcode.immediateOffset = immediateOffset;

		return std::bit_cast<uint32_t>(opcode);
	}

	static constexpr SingleDataTransfer_t fromOpcode(const uint32_t opcode)
	{
		return std::bit_cast<SingleDataTransfer_t>(opcode);
	}

	class SingleDataTransferIPrDWNS {
	public:
		
		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::ENoWriteback) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			uint32_t offset = op.offset;
			offset += (op.baseRegister == 15) ? 4 : 0; //for PC as offset, remember that PC is behind
			uint32_t calculated = regs[op.baseRegister];
			if (op.destinationRegister == 15)
				regs[op.destinationRegister] += 8;
			writeToAddress32(calculated, regs[op.destinationRegister]);
			calculated -= offset;
			if (op.destinationRegister == 15)
				regs[op.destinationRegister] -= 8;
			regs[op.baseRegister] = calculated;
		}
	};

	class SingleDataTransferIPoUBNS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::ENoWriteback) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			assert(op.destinationRegister != 15);
			int calculated = 0;
			int offset = op.offset;
			offset += (op.baseRegister == 15) ? 4 : 0;
			int oldReg = regs[op.baseRegister];
			regs[op.baseRegister] += offset;
			calculated = regs[op.baseRegister];
			regs[op.baseRegister] = oldReg;
			if (op.destinationRegister == op.baseRegister)
				regs[op.destinationRegister] -= offset;
			writeToAddress(calculated, regs[op.destinationRegister]);
			if (op.destinationRegister == op.baseRegister)
				regs[op.destinationRegister] += offset;
		}
	};

}