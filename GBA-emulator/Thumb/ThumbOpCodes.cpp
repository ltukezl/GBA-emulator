#include <array>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <utility>
#include <print>

#include "Arm/Swi.hpp"
#include "GBAcpu.h"
#include "Thumb/ThumbOpCodes.h"
#include "Thumb/ThumbOpcodes/AddSubstract.hpp"
#include "Thumb/ThumbOpcodes/AddToSp.hpp"
#include "Thumb/ThumbOpcodes/AluOps.hpp"
#include "Thumb/ThumbOpcodes/BranchLink.hpp"
#include "Thumb/ThumbOpcodes/ConditionalBranch.hpp"
#include "Thumb/ThumbOpcodes/HiRegOps.hpp"
#include "Thumb/ThumbOpcodes/LoadAddress.hpp"
#include "Thumb/ThumbOpcodes/LoadStoreHalfword.hpp"
#include "Thumb/ThumbOpcodes/LoadStoreImm.hpp"
#include "Thumb/ThumbOpcodes/LoadStoreRegOffset.hpp"
#include "Thumb/ThumbOpcodes/LoadStoreSignExtend.hpp"
#include "Thumb/ThumbOpcodes/MovCmpAddSubImm.hpp"
#include "Thumb/ThumbOpcodes/MoveShiftedRegister.hpp"
#include "Thumb/ThumbOpcodes/MultipleLoad.hpp"
#include "Thumb/ThumbOpcodes/MultipleStore.hpp"
#include "Thumb/ThumbOpcodes/PcRelativeLoad.hpp"
#include "Thumb/ThumbOpcodes/PopRegisters.hpp"
#include "Thumb/ThumbOpcodes/PushRegisters.hpp"
#include "Thumb/ThumbOpcodes/SpRelativeOps.hpp"
#include "Thumb/ThumbOpcodes/UnconditionalBranch.hpp"

template<uint16_t op>
static consteval auto decode_table()
{
	if constexpr (AddSubThumb::isThisOpcode(op))
		return &(AddSubThumb::execute<AddSubThumb::mask(op)>);
	else if constexpr (MoveShiftedRegister::isThisOpcode(op))
		return &(MoveShiftedRegister::execute<MoveShiftedRegister::mask(op)>);
	else if constexpr (MovCmpAddSubImm::isThisOpcode(op))
		return &(MovCmpAddSubImm::execute<MovCmpAddSubImm::mask(op)>);
	else if constexpr (AluOps::isThisOpcode(op))
		return &(AluOps::execute<AluOps::mask(op)>);
	else if constexpr (HighRegOps::isThisOpcode(op))
		return &(HighRegOps::execute<HighRegOps::mask(op)>);
	else if constexpr (PcRelativeLoad::isThisOpcode(op))
		return &(PcRelativeLoad::execute);
	else if constexpr (PopRegisters::isThisOpcode(op))
		return &(PopRegisters::execute);
	else if constexpr (PushRegisters::isThisOpcode(op))
		return &(PushRegisters::execute);
	else if constexpr (LoadStoreReg::isThisOpcode(op))
		return &(LoadStoreReg::execute<LoadStoreReg::mask(op)>);
	else if constexpr (LoadStoreImm::isThisOpcode(op))
		return &(LoadStoreImm::execute<LoadStoreImm::mask(op)>);
	else if constexpr (SpRelativeOps::isThisOpcode(op))
		return &(SpRelativeOps::execute<SpRelativeOps::mask(op)>);
	else if constexpr (AddToSp::isThisOpcode(op))
		return &(AddToSp::execute<AddToSp::mask(op)>);
	else if constexpr (LoadStoreHalfword::isThisOpcode(op))
		return &(LoadStoreHalfword::execute<LoadStoreHalfword::mask(op)>);
	else if constexpr (LoadStoreSignExtend::isThisOpcode(op))
		return &(LoadStoreSignExtend::execute<LoadStoreSignExtend::mask(op)>);
	else if constexpr (LoadAddress::isThisOpcode(op))
		return &(LoadAddress::execute<LoadAddress::mask(op)>);
	else if constexpr (MultipleLoad::isThisOpcode(op))
		return &(MultipleLoad::execute);
	else if constexpr (MultipleStore::isThisOpcode(op))
		return &(MultipleStore::execute);
	else if constexpr (Swi::isThisOpcode_thumb(op))
		return &(Swi::execute);
	else if constexpr (UnconditionalBranch::isThisOpcode(op))
		return &(UnconditionalBranch::execute);
	else if constexpr (ConditionalBranch::isThisOpcode(op))
		return &(ConditionalBranch::execute<ConditionalBranch::mask(op)>);
	else if constexpr (BranchLink::isThisOpcode(op))
		return &(BranchLink::execute<BranchLink::mask(op)>);
	else
		return &(AluOps::execute<AluOps::mask(op)>);
}

template<typename T, std::size_t... Opcodes>
static consteval auto insert_to_table(T& arr, std::index_sequence<Opcodes...>)
{
	((arr[Opcodes] = decode_table<static_cast<uint16_t>(Opcodes) << 6 >()), ...);
}

static constexpr std::array thumb_dispatch = { []() consteval
 {
	std::array <decltype(&AddSubThumb::execute<0>), 1024> tmp {};
	insert_to_table(tmp, std::make_index_sequence<tmp.size()>{});
	return tmp;
}() };

#include <iostream>
#include <fstream>
#include <cassert>

std::string thumb_disassembly(const uint32_t program_counter, const uint16_t opcode)
{
	if (AddSubThumb::isThisOpcode(opcode))
		return AddSubThumb::disassemble(opcode);
	else if (MoveShiftedRegister::isThisOpcode(opcode))
		return MoveShiftedRegister::disassemble(opcode);
	else if (MovCmpAddSubImm::isThisOpcode(opcode))
		return MovCmpAddSubImm::disassemble(opcode);
	else if (AluOps::isThisOpcode(opcode))
		return AluOps::disassemble(opcode);
	else if (HighRegOps::isThisOpcode(opcode))
		return HighRegOps::disassemble(opcode);
	else if (PcRelativeLoad::isThisOpcode(opcode))
		return PcRelativeLoad::disassemble(opcode);
	else if (PopRegisters::isThisOpcode(opcode))
		return PopRegisters::disassemble(opcode);
	else if (PushRegisters::isThisOpcode(opcode))
		return PushRegisters::disassemble(opcode);
	else if (LoadStoreReg::isThisOpcode(opcode))
		return LoadStoreReg::disassemble(opcode);
	else if (LoadStoreImm::isThisOpcode(opcode))
		return LoadStoreImm::disassemble(opcode);
	else if (SpRelativeOps::isThisOpcode(opcode))
		return SpRelativeOps::disassemble(opcode);
	else if (AddToSp::isThisOpcode(opcode))
		return AddToSp::disassemble(opcode);
	else if (LoadAddress::isThisOpcode(opcode))
		return LoadAddress::disassemble(opcode);
	else if (LoadStoreHalfword::isThisOpcode(opcode))
		return LoadStoreHalfword::disassemble(opcode);
	else if (LoadStoreSignExtend::isThisOpcode(opcode))
		return LoadStoreSignExtend::disassemble(opcode);
	else if (MultipleLoad::isThisOpcode(opcode))
		return MultipleLoad::disassemble(opcode);
	else if (MultipleStore::isThisOpcode(opcode))
		return MultipleStore::disassemble(opcode);
	else if (Swi::isThisOpcode_thumb(opcode))
		return Swi::disassemble(opcode);
	else if (ConditionalBranch::isThisOpcode(opcode))
		return ConditionalBranch::disassemble(program_counter, opcode);
	else if (UnconditionalBranch::isThisOpcode(opcode))
		return UnconditionalBranch::disassemble(program_counter, opcode);
	else
		return BranchLink::disassemble(program_counter, opcode);
}

void runSingleStepTests()
{
	Registers registers;
	const std::string game = "../ARM7TDMI/v1/thumb_push_pop.json";
	std::ifstream ifs(game);
	const auto jf = nlohmann::json::parse(ifs);
	for (const auto& json : jf)
	{
		for (auto& j : json["transactions"])
		{
			if(j["kind"] == 0 || j["kind"] == 1)
				writeToAddress32(j["addr"], j["data"]);
		}

		const auto initials = json["initial"];
		size_t idx = 0;
		for (auto& initial_R : initials["R"])
		{
			*registers.usrSys[idx] = initial_R.get<uint32_t>();
			idx++;
		}

		registers[15] = json["base_addr"][0].get<uint32_t>();

		idx = 0;
		for (auto& initial_R : initials["R_fiq"])
		{
			registers.fiqBanked[idx] = initial_R.get<uint32_t>();
			idx++;
		}

		idx = 0;
		for (auto& initial_R : initials["R_svc"])
		{
			registers.svcBanked[idx] = initial_R.get<uint32_t>();
			idx++;
		}

		idx = 0;
		for (auto& initial_R : initials["R_abt"])
		{
			registers.abtBanked[idx] = initial_R.get<uint32_t>();
			idx++;
		}

		idx = 0;
		for (auto& initial_R : initials["R_irq"])
		{
			registers.irqBanked[idx] = initial_R.get<uint32_t>();
			idx++;
		}

		idx = 0;
		for (auto& initial_R : initials["R_und"])
		{
			registers.undBanked[idx] = initial_R.get<uint32_t>();
			idx++;
		}

		registers.m_cpsr.val = initials["CPSR"].get<uint32_t>();
		registers.updateMode(registers.getMode());

		registers.sprs_usr = 0;
		registers.sprs_fiq = initials["SPSR"][0].get<uint32_t>();
		registers.sprs_svc = initials["SPSR"][1].get<uint32_t>();
		registers.sprs_abt = initials["SPSR"][2].get<uint32_t>();
		registers.sprs_irq = initials["SPSR"][3].get<uint32_t>();
		registers.sprs_udf = initials["SPSR"][4].get<uint32_t>();

		uint16_t opcode = initials["pipeline"][0].get<uint16_t>();

		std::cout << opcode << " " << registers[0] << " " << thumb_disassembly(registers[15], opcode) << "\n";

		registers[15] += 2;
		thumb_dispatch[opcode >> 6](registers, opcode);
		registers[15] += 4;

		idx = 0;
		for (auto& j : json["transactions"])
		{
			if (j["kind"] == 2)
			{
				const auto val = loadFromAddress32(j["addr"].get<uint32_t>());
				std::println("{} {}", j["addr"].get<uint32_t>(), val);
				const auto expected = j["data"].get<uint32_t>();
				assert(val == expected);
				idx++;
			}
		}


		const auto finals = json["final"];

		idx = 0;
		for (auto& initial_R : finals["R"])
		{
			assert(*registers.usrSys[idx] == initial_R.get<uint32_t>());
			idx++;
		}

		idx = 0;
		for (auto& initial_R : finals["R_fiq"])
		{
			assert(registers.fiqBanked[idx] == initial_R.get<uint32_t>());
			idx++;
		}

		idx = 0;
		for (auto& initial_R : finals["R_svc"])
		{
			assert(registers.svcBanked[idx] == initial_R.get<uint32_t>());
			idx++;
		}

		idx = 0;
		for (auto& initial_R : finals["R_abt"])
		{
			assert(registers.abtBanked[idx] == initial_R.get<uint32_t>());
			idx++;
		}

		idx = 0;
		for (auto& initial_R : finals["R_irq"])
		{
			assert(registers.irqBanked[idx] == initial_R.get<uint32_t>());
			idx++;
		}

		idx = 0;
		for (auto& initial_R : finals["R_und"])
		{
			assert(registers.undBanked[idx] == initial_R.get<uint32_t>());
			idx++;
		}

		if (((opcode >> 6) & 0xf) == 0b1101)
			continue;

		assert(registers.m_cpsr.val == finals["CPSR"].get<uint32_t>());
		assert(registers.sprs_fiq == finals["SPSR"][0].get<uint32_t>());
		assert(registers.sprs_svc == finals["SPSR"][1].get<uint32_t>());
		assert(registers.sprs_abt == finals["SPSR"][2].get<uint32_t>());
		assert(registers.sprs_irq == finals["SPSR"][3].get<uint32_t>());
		assert(registers.sprs_udf == finals["SPSR"][4].get<uint32_t>());
	}
}

void thumbExecute(const uint16_t opcode){
	cycles += 1;
	thumb_dispatch[opcode >> 6](r, opcode);
}

