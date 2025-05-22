#pragma once
#include <bit>
#include <cstdint>
#include <cassert>
#include <format>
#include <iostream>

#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"
#include "SingleDataTransfer.hpp"

namespace SingleDataTransfer {

	// ----------
	// PRE FUNCS STORE
	// ----------

	class SingleDataTransferIPrDWNS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::ENoWriteback) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
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
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::ENoWriteback) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
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
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::ENoWriteback) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
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
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::ENoWriteback) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
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
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::EWriteback) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] - op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			writeToAddress32(calculated, regs[op.destinationRegister]);
			destinationRegisterBug(op, regs);

			if constexpr (DebugPrints)
			{

			}
		}
	};

	class SingleDataTransferIPrDBWS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::EWriteback) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] - op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			writeToAddress(calculated, regs[op.destinationRegister]);
			destinationRegisterBug(op, regs);
		}
	};

	class SingleDataTransferIPrUWWS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::EWriteback) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] + op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			writeToAddress32(calculated, regs[op.destinationRegister]);
			destinationRegisterBug(op, regs);
		}
	};

	class SingleDataTransferIPrUBWS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.writeBack == writeBack_t::EWriteback) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] + op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			writeToAddress(calculated, regs[op.destinationRegister]);
			destinationRegisterBug(op, regs);
		}
	};


	// ----------------
	// POST FUNCS LOAD
	// ---------------

	class SingleDataTransferIPrDWNL {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.writeBack == writeBack_t::ENoWriteback) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] - op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.destinationRegister] = loadFromAddress32(calculated);

			if constexpr (DebugPrints)
			{

			}
		}
	};

	class SingleDataTransferIPrDBNL {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.writeBack == writeBack_t::ENoWriteback) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] - op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.destinationRegister] = loadFromAddress(calculated);
		}
	};

	class SingleDataTransferIPrUWNL {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.writeBack == writeBack_t::ENoWriteback) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] + op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.destinationRegister] = loadFromAddress32(calculated);
		}
	};

	class SingleDataTransferIPrUBNL {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.writeBack == writeBack_t::ENoWriteback) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] + op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.destinationRegister] = loadFromAddress(calculated);
		}
	};

	class SingleDataTransferIPrDWWL {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.writeBack == writeBack_t::EWriteback) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] - op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			regs[op.destinationRegister] = loadFromAddress32(calculated);

			if constexpr (DebugPrints)
			{

			}
		}
	};

	class SingleDataTransferIPrDBWL {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.writeBack == writeBack_t::EWriteback) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] - op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			regs[op.destinationRegister] = loadFromAddress(calculated);
		}
	};

	class SingleDataTransferIPrUWWL {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.writeBack == writeBack_t::EWriteback) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] + op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			regs[op.destinationRegister] = loadFromAddress32(calculated);
		}
	};

	class SingleDataTransferIPrUBWL {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.writeBack == writeBack_t::EWriteback) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister] + op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			regs[op.destinationRegister] = loadFromAddress(calculated);
		}
	};

	// ----------------
	// POST FUNCS STORE
	// ---------------

	class SingleDataTransferIPoUWNS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
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
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
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
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
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
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
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
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister];
			const auto ret = loadFromAddress32(calculated);
			calculated += op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			regs[op.destinationRegister] = ret;
			destinationRegisterBug(op, regs);
		}
	};

	class SingleDataTransferIPoUBNL {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::EAdd) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister];
			const auto ret = loadFromAddress(calculated);
			calculated += op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			regs[op.destinationRegister] = ret;
			destinationRegisterBug(op, regs);
		}
	};

	class SingleDataTransferIPoDWNL {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.byteTransfer == byteWord_t::EWord) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister];
			const auto ret = loadFromAddress32(calculated);
			calculated -= op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			regs[op.destinationRegister] = ret;
			destinationRegisterBug(op, regs);
		}
	};

	class SingleDataTransferIPoDBNL {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.byteTransfer == byteWord_t::EByte) &&
				(op.addOffset == upDown_t::ESubstract) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			uint32_t calculated = regs[op.baseRegister];
			const auto ret = loadFromAddress(calculated);
			calculated -= op.offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			regs[op.destinationRegister] = ret;
			destinationRegisterBug(op, regs);
		}
	};
}