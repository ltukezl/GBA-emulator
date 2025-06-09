#ifndef SDT_H
#define SDT_H

#include <bit>
#include <cassert>
#include <Constants.h>
#include <cstdint>
#include <format>
#include <type_traits>

#include "cplusplusRewrite/BarrelShifterDecoder.h"
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

	static constexpr SingleDataTransfer_t fromOpcode(const uint32_t opcode)
	{
		assert(std::is_trivially_copyable_v<SingleDataTransfer_t>);
		assert(sizeof(SingleDataTransfer_t) == sizeof(uint32_t));
		return {
			.offset = static_cast<uint32_t>(opcode & 0xFFF),
			.destinationRegister = static_cast<uint32_t>((opcode >> 12) & 0xF),
			.baseRegister = static_cast<uint32_t>((opcode >> 16) & 0xF),
			.loadBit = static_cast<loadStore_t>((opcode >> 20) & 0x1),
			.writeBack = static_cast<writeBack_t>((opcode >> 21) & 0x1),
			.byteTransfer = static_cast<byteWord_t>((opcode >> 22) & 0x1),
			.addOffset = static_cast<upDown_t>((opcode >> 23) & 0x1),
			.preIndexing = static_cast<prePost_t>((opcode >> 24) & 0x1),
			.immediateOffset = static_cast<immediate_t>((opcode >> 25) & 0x1),
			.unused = static_cast<uint32_t>((opcode >> 26) & 0x3),
			.executionCondition = static_cast<uint32_t>((opcode >> 28) & 0xF)
		};
	}

	static constexpr auto mask(const uint32_t opcode)
	{
		return opcode & (0x3F << 20);
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

	template<SingleDataTransfer_t op>
	static consteval auto memLoadOp()
	{
		if constexpr (op.byteTransfer == byteWord_t::EWord)
		{
			return &loadFromAddress32;
		}
		else
		{
			return &loadFromAddress;
		}
	}

	template<SingleDataTransfer_t op>
	static consteval auto memStoreOp()
	{
		if constexpr (op.byteTransfer == byteWord_t::EWord)
		{
			return &writeToAddress32;
		}
		else
		{
			return  &writeToAddress;
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

#endif