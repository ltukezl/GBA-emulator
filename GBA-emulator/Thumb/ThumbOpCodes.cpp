#include <array>
#include <bit>
#include <cstdint>
#include <print>
#include <utility>

#include "Arm/ArmOpcodes/SingleDataTransfer.hpp"
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
#include "Thumb/ThumbOpcodes/AddToSp.hpp"
#include "Thumb/ThumbOpcodes/AluOps.hpp"
#include "Thumb/ThumbOpcodes/BranchLink.hpp"
#include "Thumb/ThumbOpcodes/LoadAddress.hpp"
#include "Thumb/ThumbOpcodes/MovCmpAddSubImm.hpp"
#include "Thumb/ThumbOpcodes/MoveShiftedRegister.hpp"
#include "Thumb/ThumbOpcodes/PcRelativeLoad.hpp"
#include "Thumb/ThumbOpcodes/PopRegisters.hpp"
#include "Thumb/ThumbOpcodes/PushRegisters.hpp"
#include "Thumb/ThumbOpcodes/SpRelativeOps.hpp"

static void bx(int& saveTo, int immidiate, int immidiate2){
	r[TRegisters::EProgramCounter] = immidiate2 & ~1;
	r.m_cpsr.thumb = immidiate2 & 1;
}

void(*hlOps[4])(int&, int, int) = { Add, Cmp, Mov, bx };

static void hiRegOperations(uint16_t opcode){

	union hiRegOps op = { opcode };
	uint8_t newDestinationReg = 8 * op.destHiBit + op.destination;
	uint32_t operand1 = r[newDestinationReg];
	uint32_t operand2 = r[op.source];

	if (op.source == 15){
		operand2 += 2;
		operand2 &= ~1;
	}

	if (newDestinationReg == 15){
		operand1 += 2;
		operand2 &= ~1;
	}

	hlOps[op.instruction]((int&)r[newDestinationReg], operand1, operand2);

	cycles += S_cycles;
}

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

static void loadStoreHalfword(uint16_t opcode){
	union loadStoreHalfWord op = { opcode };
	int immediate = op.offset << 1; //half word alignment, 5 bits to 6 bits last bit is 0
	if (op.loadFlag)
		r[op.destSourceReg] = loadFromAddress16(r[op.baseReg] + immediate);
	else
		writeToAddress16(r[op.baseReg] + immediate, r[op.destSourceReg]);
}

static void multiLoad(uint16_t opcode){
	int immediate = opcode & 0xFF;
	int loadFlag = (opcode >> 11) & 1;
	int baseReg = (opcode >> 8) & 7;

	uint32_t savedAddr = 0;
	bool rInList = immediate & (1 << baseReg);
	bool first = (immediate & ((1 << baseReg) - 1)) == 0;

	if (loadFlag){
		if (immediate == 0){
			r[TRegisters::EProgramCounter] = (loadFromAddress32(r[baseReg])) & ~1;
			r[baseReg] += 0x40;
		}
		else {
			for (int i = 0; i < 8; i++){
				if (immediate & 1){
					r[i] = loadFromAddress32(r[baseReg]);
					r[baseReg] += 4;
				}
				immediate >>= 1;
			}
		}
	}
	else{
		if (immediate == 0){
			writeToAddress32(r[baseReg], r[PC] + 4);
			r[baseReg] += 0x40;
		}
		else{
			for (int i = 0; i < 8; i++){
				if (immediate & 1){
					if (i == baseReg)
						savedAddr = r[baseReg];
					writeToAddress32(r[baseReg], r[i]);
					r[baseReg] += 4;
				}
				immediate >>= 1;
			}
		}
		if (rInList && !first)
			writeToAddress32(savedAddr, r[baseReg]);
	}
	cycles += 1;
}

static void conditionalBranch(uint16_t opcode){
	union conditionalBranchOp op = { opcode };
	const uint32_t location = ((op.immediate << 1) + 2);
	r[PC] += conditions[op.condition]() ? location : 0;

	//if(debug)
	//	 std::print("B{} #0x{:x}", condition_strings[op.condition], location);
}

static void unconditionalBranch(uint16_t opcode){
	int immediate = (opcode & 0x7FF) << 1;
	r[PC] += signExtend<12>(immediate) +2;
}

static void branchLink(uint16_t opcode){
	int HLOffset = (opcode >> 11) & 1;
	int immediate = (opcode & 0x7FF);
	if (!HLOffset)
	{
		r[LR] = (signExtend<11>(immediate) << 12) + r[PC] + 2;
		std::println("{:x}", r[ELinkRegisterLR]);
	}
	else{
		int nextInstruction = r[PC] + 1;
		r[TRegisters::EProgramCounter] = r[LR] + (immediate << 1);
		r[LR] = nextInstruction | 1;
		std::println("{:x} {:x}", r[EProgramCounter], r[ELinkRegisterLR]);
	}
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
	else if constexpr (PcRelativeLoad::isThisOpcode(op))
		return &(PcRelativeLoad::execute<PcRelativeLoad::mask(op)>);
	else if constexpr (PopRegisters::isThisOpcode(op))
		return &(PopRegisters::execute<PopRegisters::mask(op)>);
	else if constexpr (PushRegisters::isThisOpcode(op))
		return &(PushRegisters::execute<PushRegisters::mask(op)>);
	else if constexpr (SpRelativeOps::isThisOpcode(op))
		return &(SpRelativeOps::execute<SpRelativeOps::mask(op)>);
	else if constexpr (AddToSp::isThisOpcode(op))
		return &(AddToSp::execute<AddToSp::mask(op)>);
	else if constexpr (LoadAddress::isThisOpcode(op))
		return &(LoadAddress::execute<LoadAddress::mask(op)>);
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
	
	/*
	if (AddSubThumb::isThisOpcode(opcode))
		std::println("{}", AddSubThumb::disassemble(opcode));
	else if (MoveShiftedRegister::isThisOpcode(opcode))
		std::println("{}", MoveShiftedRegister::disassemble(opcode));
	else if (MovCmpAddSubImm::isThisOpcode(opcode))
		std::println("{}", MovCmpAddSubImm::disassemble(opcode));
	else if (AluOps::isThisOpcode(opcode))
		std::println("{}", AluOps::disassemble(opcode));
	else if (PcRelativeLoad::isThisOpcode(opcode))
		std::println("{}", PcRelativeLoad::disassemble(opcode));
	*/

	if (BranchLink::isThisOpcode(opcode))
		std::println("{}", BranchLink::disassemble(r, opcode));

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
			hiRegOperations(opcode);
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
		subType = (opcode >> 12) & 1;
		switch (subType){
		case 0: //load half word reg - imm
			loadStoreHalfword(opcode);
			break;

		case 1: //load SP relative
			thumb_dispatch[opcode >> 6](r, opcode);
			break;
		}
		break;

	case 5:
		thumb_dispatch[opcode >> 6](r, opcode);
		break;

	case 6:
		subType = (opcode >> 12) & 1;
		switch (subType){
		case 0: // multiple load / store
			multiLoad(opcode);
			break;

		case 1:
			int condition = (opcode >> 8) & 0x0F;
			switch (condition)
			{
			case 15: //software interrupt
				interruptController();
				break;
			default:  //conditional branch
				conditionalBranch(opcode);
				break;
			}
			break;
		}
		break;

	case 7:
		subType = (opcode >> 12) & 1;
		switch (subType){
		case 0: //unconditional branch
			unconditionalBranch(opcode);
			break;

		case 1: //branch and link
			thumb_dispatch[opcode >> 6](r, opcode);
			break;
		}
		break;
	}
}

