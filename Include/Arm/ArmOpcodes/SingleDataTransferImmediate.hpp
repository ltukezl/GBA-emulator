#pragma once
#include <cstdint>

#include "cplusplusRewrite/HwRegisters.h"
#include "SingleDataTransfer.hpp"

namespace SingleDataTransfer {
	// ----------
	// PRE FUNCS STORE
	// ----------

	class SingleDataTransferIPrS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		template<uint32_t opcode_iter>
		static void execute(Registers& regs, const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			constexpr auto c_op = fromOpcode(opcode_iter);
			constexpr auto storeOperation = memStoreOp<c_op>();
			uint32_t offset = op.offset;
			if constexpr (c_op.addOffset == upDown_t::ESubstract)
			{
				offset = -offset;
			}

			uint32_t calculated = regs[op.baseRegister];
			calculated += offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;

			if constexpr (c_op.writeBack == writeBack_t::EWriteback)
				regs[op.baseRegister] = calculated;

			storeOperation(calculated, regs[op.destinationRegister]);
			destinationRegisterBug(op, regs);
		}
	};

	// ----------------
	// POST FUNCS STORE
	// ---------------

	class SingleDataTransferIPoS {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::EStore)  && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		template<uint32_t opcode_iter>
		static void execute(Registers& regs, const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			constexpr auto c_op = fromOpcode(opcode_iter);
			constexpr auto storeOperation = memStoreOp<c_op>();
			uint32_t offset = op.offset;
			if constexpr (c_op.addOffset == upDown_t::ESubstract)
			{
				offset = -offset;
			}

			uint32_t calculated = regs[op.baseRegister];
			storeOperation(calculated, regs[op.destinationRegister]);
			calculated += offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			destinationRegisterBug(op, regs);
		}
	};

// ----------------
// PRE FUNCS LOAD
// ---------------

	class SingleDataTransferIPrL
	{
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.preIndexing == prePost_t::EPre) && (op.immediateOffset == immediate_t::EImmediate);
		}

		template<uint32_t opcode_iter>
		static void execute(Registers& regs, const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			constexpr auto c_op = fromOpcode(opcode_iter);
			constexpr auto loadOperation = memLoadOp<c_op>();
			uint32_t offset = op.offset;
			if constexpr (c_op.addOffset == upDown_t::ESubstract)
			{
				offset = -offset;
			}

			uint32_t calculated = regs[op.baseRegister];
			calculated += offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;

			if constexpr (c_op.writeBack == writeBack_t::EWriteback)
				regs[op.baseRegister] = calculated;

			regs[op.destinationRegister] = loadOperation(calculated, false);
		}
	};

	// ----------------
	// POST FUNCS LOAD
	// ---------------

	class SingleDataTransferIPoL {
	public:

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) && (op.preIndexing == prePost_t::EPost) && (op.immediateOffset == immediate_t::EImmediate);
		}

		template<uint32_t opcode_iter>
		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			constexpr auto c_op = fromOpcode(opcode_iter);
			constexpr auto loadOperation = memLoadOp<c_op>();
			uint32_t offset = op.offset;
			if constexpr (c_op.addOffset == upDown_t::ESubstract)
			{
				offset = -offset;
			}

			uint32_t calculated = regs[op.baseRegister];
			const auto ret = loadOperation(calculated, false);
			calculated += offset;
			calculated += (op.baseRegister == 15) ? 4 : 0;
			regs[op.baseRegister] = calculated;
			regs[op.destinationRegister] = ret;
			destinationRegisterBug(op, regs);
		}
	};

	using SDT_t = decltype(&SingleDataTransferIPoL::execute<0>);
}