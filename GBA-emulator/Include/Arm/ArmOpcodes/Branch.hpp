#pragma once
#include <bit>
#include <cassert>
#include <cstdint>
#include <format>

#include "Constants.h"
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

		static constexpr uint32_t fromFields(const uint32_t offset, BL_t link) {
			ArmBranch_t opcode{};
			static_assert(sizeof(ArmBranch_t) == sizeof(uint32_t));
			opcode.offset = offset;
			opcode.link = link;
			opcode.unused = 0b101;
			opcode.condition = 0xe;

			return std::bit_cast<uint32_t>(opcode);
		}

		static constexpr ArmBranch_t fromOpcode(const uint32_t opcode)
		{
			assert(std::is_trivially_copyable_v<ArmBranch_t>);
			assert(sizeof(ArmBranch_t) == sizeof(uint32_t));
			return std::bit_cast<ArmBranch_t>(opcode);
		}

		static constexpr bool isThisOpcode(const uint32_t opcode)
		{
			auto op = fromOpcode(opcode);
			return (op.unused == 0b101);
		}

		static void execute(Registers& regs, const uint32_t opcode) {
			const auto op = std::bit_cast<ArmBranch_t>(opcode);
			if (op.link == BL_t::EBranchAndLink)
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

		static constexpr uint32_t fromFields(const uint32_t branchReg) {
			BxOP opcode{};
			static_assert(sizeof(BxOP) == sizeof(uint32_t));
			opcode.rn = branchReg;
			opcode.unused = 0x12FFF1;
			opcode.condition = 0xe;

			return std::bit_cast<uint32_t>(opcode);
		}

		static constexpr BxOP fromOpcode(const uint32_t opcode)
		{
			assert(std::is_trivially_copyable_v<BxOP>);
			assert(sizeof(BxOP) == sizeof(uint32_t));
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