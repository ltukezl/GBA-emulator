#include <array>
#include <bit>
#include <cstdint>
#include <print>
#include <utility>

#include "Arm/ArmOpcodes/SingleDataTransfer.hpp"
#include "Arm/Swi.hpp"
#include "CommonOperations/arithmeticOps.h"
#include "CommonOperations/conditions.h"
#include "CommonOperations/logicalOps.h"
#include "Constants.h"
#include "cplusplusRewrite/HwRegisters.h"
#include "GBAcpu.h"
#include "Interrupt/interrupt.h"
#include "Memory/memoryOps.h"
#include "Thumb/ThumbOpCodes.h"
#include "Thumb/ThumbOpcodes/AddSubstract.hpp"
#include "Thumb/ThumbOpcodes/MultipleStore.hpp"
#include "Thumb/ThumbOpcodes/MultipleLoad.hpp"
#include "Thumb/ThumbOpcodes/AddToSp.hpp"
#include "Thumb/ThumbOpcodes/AluOps.hpp"
#include "Thumb/ThumbOpcodes/BranchLink.hpp"
#include "Thumb/ThumbOpcodes/ConditionalBranch.hpp"
#include "Thumb/ThumbOpcodes/HiRegOps.hpp"
#include "Thumb/ThumbOpcodes/LoadAddress.hpp"
#include "Thumb/ThumbOpcodes/LoadStoreHalfword.hpp"
#include "Thumb/ThumbOpcodes/MovCmpAddSubImm.hpp"
#include "Thumb/ThumbOpcodes/MoveShiftedRegister.hpp"
#include "Thumb/ThumbOpcodes/PcRelativeLoad.hpp"
#include "Thumb/ThumbOpcodes/PopRegisters.hpp"
#include "Thumb/ThumbOpcodes/PushRegisters.hpp"
#include "Thumb/ThumbOpcodes/SpRelativeOps.hpp"
#include "Thumb/ThumbOpcodes/UnconditionalBranch.hpp"

static void loadStoreRegOffset(uint16_t opcode){
	union loadStoreRegOffset op = { opcode };
	uint32_t totalAddress = r[op.baseReg] + r[op.offsetReg];

	if (op.loadBit && op.byteSize)
		r[op.destSourceReg] = loadFromAddress(totalAddress);
	else if (op.loadBit && !op.byteSize)
		r[op.destSourceReg] = loadFromAddress32(totalAddress);
	else if (!op.loadBit && op.byteSize)
		writeToAddress(totalAddress, r[op.destSourceReg]);
	else
		writeToAddress32(totalAddress, r[op.destSourceReg]);
}

static void loadStoreSignExtend(uint16_t opcode){
	union loadStoreSignExtended op = { opcode };
	uint32_t totalAddress = r[op.baseReg] + r[op.offsetReg];

	if (op.halfWord && op.extend){
		if (totalAddress & 1){
			r[op.destSourceReg] = signExtend<8>(loadFromAddress16(totalAddress));
			if (r[op.destSourceReg] & 0x80) //sign bit on
				r[op.destSourceReg] |= 0xFFFFFF00;
		}
		else{
			r[op.destSourceReg] = signExtend<16>(loadFromAddress16(totalAddress));
		}
	}
	else if (op.halfWord && !op.extend)
		r[op.destSourceReg] = loadFromAddress16(totalAddress);
	else if (!op.halfWord && op.extend)
		r[op.destSourceReg] = signExtend<8>(loadFromAddress(totalAddress));
	else
		writeToAddress16(totalAddress, r[op.destSourceReg]);
}

static void loadStoreImm(uint16_t opcode){
	const auto op = std::bit_cast<loadStoreImmediate>(opcode);
	const uint32_t shift = op.byteSize ? 0 : 2;
	const uint8_t immediate = op.offset << shift;
	uint32_t totalAddress = r[op.baseReg] + immediate;

	if (op.loadFlag && op.byteSize)
		r[op.destSourceReg] = loadFromAddress(totalAddress);
	else if (op.loadFlag && !op.byteSize)
		r[op.destSourceReg] = loadFromAddress32(totalAddress);
	else if (!op.loadFlag && op.byteSize)
		writeToAddress(totalAddress, r[op.destSourceReg]);
	else
		writeToAddress32(totalAddress, r[op.destSourceReg]);
}

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
	else if constexpr (SpRelativeOps::isThisOpcode(op))
		return &(SpRelativeOps::execute<SpRelativeOps::mask(op)>);
	else if constexpr (AddToSp::isThisOpcode(op))
		return &(AddToSp::execute<AddToSp::mask(op)>);
	else if constexpr (LoadStoreHalfword::isThisOpcode(op))
		return &(LoadStoreHalfword::execute<LoadStoreHalfword::mask(op)>);
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

void thumbExecute(uint16_t opcode){
	int subType;
	cycles += 1;
	__int16 type = (opcode & 0xE000) >> 13;
	
	
	if (AddSubThumb::isThisOpcode(opcode))
		std::println("{}", AddSubThumb::disassemble(opcode));
	else if (MoveShiftedRegister::isThisOpcode(opcode))
		std::println("{}", MoveShiftedRegister::disassemble(opcode));
	else if (MovCmpAddSubImm::isThisOpcode(opcode))
		std::println("{}", MovCmpAddSubImm::disassemble(opcode));
	else if (AluOps::isThisOpcode(opcode))
		std::println("{}", AluOps::disassemble(opcode));
	else if (HighRegOps::isThisOpcode(opcode))
		std::println("{}", HighRegOps::disassemble(opcode));
	else if (PcRelativeLoad::isThisOpcode(opcode))
		std::println("{}", PcRelativeLoad::disassemble(opcode));
	else if (PopRegisters::isThisOpcode(opcode))
		std::println("{}", PopRegisters::disassemble(opcode));
	else if (PushRegisters::isThisOpcode(opcode))
		std::println("{}", PushRegisters::disassemble(opcode));
	else if (SpRelativeOps::isThisOpcode(opcode))
		std::println("{}", SpRelativeOps::disassemble(opcode));
	else if (AddToSp::isThisOpcode(opcode))
		std::println("{}", AddToSp::disassemble(opcode));
	else if (LoadAddress::isThisOpcode(opcode))
		std::println("{}", LoadAddress::disassemble(opcode));
	else if (LoadStoreHalfword::isThisOpcode(opcode))
		std::println("{}", LoadStoreHalfword::disassemble(opcode));
	else if (MultipleLoad::isThisOpcode(opcode))
		std::println("{}", MultipleLoad::disassemble(opcode));
	else if (MultipleStore::isThisOpcode(opcode))
		std::println("{}", MultipleStore::disassemble(opcode));
	else if (ConditionalBranch::isThisOpcode(opcode))
		std::println("{}", ConditionalBranch::disassemble(r, opcode));
	else if (UnconditionalBranch::isThisOpcode(opcode))
		std::println("{}", UnconditionalBranch::disassemble(r, opcode));
	else if (BranchLink::isThisOpcode(opcode))
		std::println("{}", BranchLink::disassemble(r, opcode));
	else
		std::println("unknown op");
	

	switch (type) {
	case 0: //shifts or add or sub, maybe sign extended for immidiates?
		thumb_dispatch[opcode >> 6](r, opcode);
		break;

	case 1: // move|compare|substract|add immediate
		thumb_dispatch[opcode >> 6](r, opcode);
		break;

	case 2: //logical ops / memory load / store
		subType = (opcode >> 10) & 7;
		switch (subType){
		case 0: //logical ops reg - reg
			thumb_dispatch[opcode >> 6](r, opcode);
			break;

		case 1: //high low reg loading, branch
			thumb_dispatch[opcode >> 6](r, opcode);
			break;

		case 2: case 3: //PC relative load
			thumb_dispatch[opcode >> 6](r, opcode);
			break;

		default:
			int subType2 = (opcode >> 9) & 1;
			switch (subType2){
			case 0: //load / store with reg offset
				loadStoreRegOffset(opcode);
				break;

			case 1: //load / store sign extended byte / word
				loadStoreSignExtend(opcode);
				break;
			}
			break;
		}
		break;

	case 3: //load / store reg - imm
		loadStoreImm(opcode);
		break;

	case 4: // load store halfword reg - imm
		thumb_dispatch[opcode >> 6](r, opcode);
		break;

	case 5:
		thumb_dispatch[opcode >> 6](r, opcode);
		break;

	case 6:
		thumb_dispatch[opcode >> 6](r, opcode);
		break;

	case 7:
		thumb_dispatch[opcode >> 6](r, opcode);
	}
}

