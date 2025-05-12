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

	static inline void destinationRegisterBug(const SingleDataTransfer_t& op, Registers& regs)
	{
		if (op.destinationRegister == 15)
			regs[op.destinationRegister] -= 8;
	}


	// ----------
	// PRE FUNCS
	// ----------

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
			uint32_t calculated = regs[op.baseRegister] - op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			writeToAddress32(calculated, regs[op.destinationRegister]);
			destinationRegisterBug(op, regs);

			if constexpr (DebugPrints)
			{

			}
		}
	};

	class SingleDataTransferIPrDBNS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::ENoWriteback) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] - op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			writeToAddress(calculated, regs[op.destinationRegister]);
			destinationRegisterBug(op, regs);
		}
	};

	class SingleDataTransferIPrUWNS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::ENoWriteback) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] + op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			writeToAddress32(calculated, regs[op.destinationRegister]);
			destinationRegisterBug(op, regs);
		}
	};

	class SingleDataTransferIPrUBNS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::ENoWriteback) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] + op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			writeToAddress(calculated, regs[op.destinationRegister]);
			destinationRegisterBug(op, regs);
		}
	};

	class SingleDataTransferIPrDWWS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::EWriteback) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] - op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			writeToAddress32(calculated, regs[op.destinationRegister]);
			destinationRegisterBug(op, regs);
			regs[op.baseRegister] = calculated;

			if constexpr (DebugPrints)
			{

			}
		}
	};

	class SingleDataTransferIPrDBWS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::EWriteback) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] - op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			writeToAddress(calculated, regs[op.destinationRegister]);
			destinationRegisterBug(op, regs);
			regs[op.baseRegister] = calculated;
		}
	};

	class SingleDataTransferIPrUWWS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::EWriteback) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] + op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			writeToAddress32(calculated, regs[op.destinationRegister]);
			destinationRegisterBug(op, regs);
			regs[op.baseRegister] = calculated;
		}
	};

	class SingleDataTransferIPrUBWS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::EWriteback) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] + op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			writeToAddress(calculated, regs[op.destinationRegister]);
			destinationRegisterBug(op, regs);
			regs[op.baseRegister] = calculated;
		}
	};

	// ----------------
	// POST FUNCS STORE
	// ---------------

	class SingleDataTransferIPoUWNS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister];
			writeToAddress32(calculated, regs[op.destinationRegister]);
			calculated += op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			destinationRegisterBug(op, regs);
		}
	};

	class SingleDataTransferIPoUBNS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister];
			writeToAddress(calculated, regs[op.destinationRegister]);
			calculated += op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			destinationRegisterBug(op, regs);
		}
	};

	class SingleDataTransferIPoDWNS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister];
			writeToAddress32(calculated, regs[op.destinationRegister]);
			calculated -= op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			destinationRegisterBug(op, regs);
		}
	};

	class SingleDataTransferIPoDBNS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister];
			writeToAddress(calculated, regs[op.destinationRegister]);
			calculated -= op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			destinationRegisterBug(op, regs);
		}
	};

	// ----------------
	// POST FUNCS LOAD
	// ---------------

	class SingleDataTransferIPoUWNL {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister];
			regs[op.destinationRegister] = loadFromAddress32(calculated);
			calculated += op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			destinationRegisterBug(op, regs);
		}
	};

	class SingleDataTransferIPoUBNL {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister];
			regs[op.destinationRegister] = loadFromAddress(calculated);
			calculated += op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			destinationRegisterBug(op, regs);
		}
	};

	class SingleDataTransferIPoDWNL {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister];
			regs[op.destinationRegister] = loadFromAddress32(calculated);
			calculated -= op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			destinationRegisterBug(op, regs);
		}
	};

	class SingleDataTransferIPoDBNL {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister];
			regs[op.destinationRegister] = loadFromAddress(calculated);
			calculated -= op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			destinationRegisterBug(op, regs);
		}
	};
}