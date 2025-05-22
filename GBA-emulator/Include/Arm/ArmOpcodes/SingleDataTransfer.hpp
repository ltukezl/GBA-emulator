#pragma once
#include <bit>
#include <cstdint>
#include <cassert>
#include <format>
#include <iostream>

#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

constexpr bool DebugPrints = true;

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
		EPost,
		EPre,
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
		static_assert(sizeof(SingleDataTransfer_t) == sizeof(uint32_t));
		opcode.offset = offset;
		opcode.destinationRegister = destinationRegister;
		opcode.baseRegister = baseRegister;
		opcode.loadBit = loadBit;
		opcode.writeBack = writeBack;
		opcode.byteTransfer = byteTransfer;
		opcode.addOffset = addOffset;
		opcode.preIndexing = preIndexing;
		opcode.immediateOffset = immediateOffset;
		opcode.unused = 1;
		opcode.executionCondition = 0xe;

		return std::bit_cast<uint32_t>(opcode);
	}

	static constexpr SingleDataTransfer_t fromOpcode(const uint32_t opcode)
	{
		assert(std::is_trivially_copyable_v<SingleDataTransfer_t>);
		assert(sizeof(SingleDataTransfer_t) == sizeof(uint32_t));
		return std::bit_cast<SingleDataTransfer_t>(opcode);
	}

	static inline void destinationRegisterBug(const SingleDataTransfer_t& op, Registers& regs)
	{
		if (op.destinationRegister == 15)
			regs[op.destinationRegister] -= 8;
	}
}