#include <array>
#include <cstdint>
#include <utility>

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

void thumbExecute(const uint16_t opcode){
	cycles += 1;
	thumb_dispatch[opcode >> 6](r, opcode);
}

