#pragma once
#include <bit>
#include <cassert>
#include <cstdint>
#include <format>
#include <type_traits>
#include <Constants.h>

#include "cplusplusRewrite/barrelShifterDecoder.h"
#include "cplusplusRewrite/HwRegisters.h"

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
		SingleDataTransfer_t opcode{};
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

	static auto makeExpression(const uint32_t opcode)
	{
		const auto op = fromOpcode(opcode);
		if (op.immediateOffset == immediate_t::EImmediate)
		{
			if (op.offset)
			{
				const auto sign = op.addOffset == upDown_t::ESubstract ? "-" : "";
				return std::format(", {}#0x{:x}", sign, op.offset);
			}
			return std::format("");
		}
		else
		{
			const auto shifter = BarrelShifterDecoder::disassemble(opcode);
			return shifter(opcode);
		}
	}

	static auto disassemble(const uint32_t opcode)
	{
		const auto op = fromOpcode(opcode);
		const auto ls = (op.loadBit == loadStore_t::ELoad) ? "LDR" : "STR";
		const auto condition = condition_strings[op.executionCondition];
		const auto bw = (op.byteTransfer == byteWord_t::EByte) ? "B" : "";
		const auto prePost1 = (op.preIndexing == prePost_t::EPre) ? "" : "]";
		const auto prePost2 = (op.preIndexing == prePost_t::EPre) ? "]" : "";
		const auto writeback = (op.writeBack == writeBack_t::EWriteback) ? "!" : "";
		return std::format("{}{}{} R{}, [R{}{}{}{}{}", ls, condition, bw, op.destinationRegister, op.baseRegister, prePost1, makeExpression(opcode), prePost2, writeback);
	}
}