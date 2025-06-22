#ifndef BRANCH_H
#define BRANCH_H

#include <bit>
#include <cstdint>
#include <format>

#include "CommonOperations/GbaStrings.hpp"
#include "cplusplusRewrite/HwRegisters.h"

namespace branches
{
	class ArmBranch
	{
	public:
		enum class BL_t : uint32_t
		{
			EBranch,
			EBranchAndLink
		};

		struct ArmBranch_t {
			int32_t offset : 24;
			BL_t link : 1;
			uint32_t unused : 3;
			uint32_t condition : 4;
		};

		static constexpr ArmBranch_t fromOpcode(const uint32_t opcode)
		{
			return {
				.offset = static_cast<int32_t>(opcode & 0xFFFFFF),
				.link = static_cast<BL_t>((opcode >> 24) & 0x1),
				.unused = static_cast<uint32_t>((opcode >> 25) & 0x7),
				.condition = static_cast<uint32_t>((opcode >> 28) & 0xF)
			};
		}

		static constexpr auto mask(const uint32_t opcode)
		{
			return opcode & (1 << 24);
		}

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			return (op.unused == 0b101);
		}

		template <uint32_t iterOpcode>
		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			constexpr auto c_op = fromOpcode(iterOpcode);
			if constexpr (c_op.link == BL_t::EBranchAndLink)
				regs[TRegisters::ELinkRegisterLR] = regs[TRegisters::EProgramCounter];
			regs[TRegisters::EProgramCounter] += 4;
			const uint32_t location = (static_cast<uint32_t>(static_cast<int32_t>(op.offset)) << 2);
			regs[TRegisters::EProgramCounter] += location;
		}

		static auto disassemble(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);
			const auto ls = op.link == BL_t::EBranchAndLink ? "BL" : "B";
			const uint32_t location = (static_cast<uint32_t>(static_cast<int32_t>(op.offset)) << 2);

			return std::format("{}{} #0x{:x}", ls, condition_strings[op.condition], location);
		}
	};

	class ArmBranchAndExhange
	{
	public:

		struct BxOP {
			uint32_t rn : 4;
			uint32_t unused : 24;
			uint32_t condition : 4;
		};

		static constexpr BxOP fromOpcode(const uint32_t opcode)
		{
			return std::bit_cast<BxOP>(opcode);
		}

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 0x12FFF1);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = fromOpcode(opcode);
			regs.m_cpsr.thumb = regs[op.rn] & 1;
			regs[TRegisters::EProgramCounter] = regs[op.rn] & ~1;
		}

		static auto disassemble(const uint32_t opcode)
		{
			const auto op = fromOpcode(opcode);

			return std::format("BX{} R{}", condition_strings[op.condition], op.rn);
		}
	};
}

#endif