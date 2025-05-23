#include <iostream>
#include <cstring>
#include <format>
#include <print>

#include "GBAcpu.h"
#include "Memory/MemoryOps.h"
#include "Interrupt/interrupt.h"
#include "Constants.h"
#include "Thumb/ThumbOpCodes.h"
#include "Arm/armopcodes.h"
#include "CommonOperations/conditions.h"
#include "CommonOperations/arithmeticOps.h"
#include "CommonOperations/logicalOps.h"
#include "Arm/ArmOpcodes/SingleDataTransfer.hpp"

void mul(int &saveTo, int immidiate, int immidiate2){
	saveTo = (immidiate2 * immidiate) & 0xFFFFFFFF;
	negative(saveTo);
	zero(saveTo);
}

void bx(int& saveTo, int immidiate, int immidiate2){
	r[TRegisters::EProgramCounter] = immidiate2 & ~1;
	r.m_cpsr.thumb = immidiate2 & 1;
}

void(*shifts_thumb[3])(int&, int, int) = { lslCond, lsrCond, asrCond };
void(*arith[2])(int&, int, int) = { Adds, Subs };
void(*movCompIpaddIpsub[4])(int&, int, int) = { Movs, Cmp, Adds, Subs };
void(*logicalOps[16])(int&, int, int) = { Ands, Eors, lslCond, lsrCond, asrCond, Adcs, Sbcs, rorCond, Tst, Neg, Cmp, Cmn, Orrs, mul, Bics, Mvns };
void(*hlOps[4])(int&, int, int) = { Add, Cmp, Mov, bx };

void moveShiftedRegister(uint16_t opcode){
	union moveShiftedRegisterOp op = { opcode };
	if (op.immediate == 0 && (op.instruction == 1 || op.instruction == 2))
		shifts_thumb[op.instruction]((int&)r[op.destination], r[op.source], 32);
	else
		shifts_thumb[op.instruction]((int&)r[op.destination], r[op.source], op.immediate);
}

void addSubFunction(uint16_t opcode){
	union addSubRegisterOp op = { opcode };
	int value = op.useImmediate ? op.regOrImmediate : r[op.regOrImmediate];
	arith[op.Sub]((int&)r[op.destination], r[op.source], value);
}

void movCompSubAddImm(uint16_t opcode){
	union movCmpAddSub op = { opcode };
	movCompIpaddIpsub[op.instruction]((int&)r[op.destination], r[op.destination], op.offset);
}

void aluOps(uint16_t opcode){
	union aluOps op = { opcode };
	logicalOps[op.instruction]((int&)r[op.destination], r[op.destination], r[op.source]);

	cycles += S_cycles;
	if (op.instruction == 2 || op.instruction == 3 || op.instruction == 4 || op.instruction == 12)
		cycles += 1;
}

void hiRegOperations(uint16_t opcode){

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

	if ((op.source == 15) || (newDestinationReg == 3))
		cycles += N_cycles + 1;
}

static void PCRelativeLoad(uint16_t opcode){
	union PCRelativeLoad op = { opcode };
	uint32_t tmpPC = (r[15] + 2) & ~2;
	tmpPC += (op.offset << 2);
	r[op.destination] = loadFromAddress32(tmpPC);

	cycles += 1;
}

void loadStoreRegOffset(uint16_t opcode){
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

	cycles += S_cycles + N_cycles + 1;
}

void loadStoreSignExtend(uint16_t opcode){
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

	cycles += S_cycles + N_cycles + 1;
}

void loadStoreImm(uint16_t opcode){
	using namespace SingleDataTransfer;
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

	cycles += S_cycles + N_cycles + 1;

	const auto ls = op.loadFlag ? loadStore_t::ELoad : loadStore_t::EStore;
	const auto bw = op.byteSize ? byteWord_t::EByte : byteWord_t::EWord;

	const auto armOp = fromFields(immediate, op.destSourceReg, op.baseReg, ls, writeBack_t::ENoWriteback, bw, upDown_t::EAdd, prePost_t::EPre, immediate_t::EImmediate);
	//std::println("{}", disassemble(armOp));
	
	cycles += S_cycles + N_cycles + 1;
	
}

void loadStoreHalfword(uint16_t opcode){
	union loadStoreHalfWord op = { opcode };
	int immediate = op.offset << 1; //half word alignment, 5 bits to 6 bits last bit is 0
	if (op.loadFlag)
		r[op.destSourceReg] = loadFromAddress16(r[op.baseReg] + immediate);
	else
		writeToAddress16(r[op.baseReg] + immediate, r[op.destSourceReg]);

	cycles += S_cycles + N_cycles + 1;
}

void loadSPRelative(uint16_t opcode){
	union SPrelativeLoad op = { opcode };
	if (op.loadFlag)
		r[op.destSourceReg] = loadFromAddress32(r[TRegisters::EStackPointer] + (op.immediate << 2));
	else
		writeToAddress32(r[TRegisters::EStackPointer] + (op.immediate << 2), r[op.destSourceReg]);

	cycles += 1;
	if (!op.loadFlag)
		cycles += N_cycles + 1;
}


void loadAddress(uint16_t opcode){
	union loadAddress op = { opcode };
	int rs = op.useSP ? r[TRegisters::EStackPointer] : ((r[TRegisters::EProgramCounter] + 2) & ~2);
	r[op.destination] = (op.immediate << 2) + rs;
}

void addOffsetToSP(uint16_t opcode){
	int loadFlag = (opcode >> 7) & 1;
	int immediate = (opcode & 0x7F) << 2;
	r[SP] += loadFlag ? -immediate : immediate;
}

void pushpop(uint16_t opcode){
	union pushPopReg op = { opcode };
	int immediate = op.regList;

	if (op.loadBit){
		for (int i = 0; i < 8; i++){
			if (immediate & 1){
				r[i] = POP();
			}
			immediate = immediate >> 1;
		}
		r[TRegisters::EProgramCounter] = op.PCRLBit ? (POP() & -2) : (r[TRegisters::EProgramCounter]);

		if (immediate & 0x80)
			cycles += 1;
	}

	else{
		if (op.PCRLBit)
			PUSH(r[LR]);
		for (int i = 0; i < 8; i++){
			if (immediate & 0x80)
				PUSH(r[7 - i]);
			immediate = immediate << 1;
		}
	}
	cycles += 1;
}

void multiLoad(uint16_t opcode){
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

void conditionalBranch(uint16_t opcode){
	union conditionalBranchOp op = { opcode };
	const uint32_t location = ((op.immediate << 1) + 2);
	r[PC] += conditions[op.condition]() ? location : 0;

	if(debug)
		 std::print("B{} #0x{:x}", condition_strings[op.condition], location);
}

void unconditionalBranch(uint16_t opcode){
	int immediate = (opcode & 0x7FF) << 1;
	r[PC] += signExtend<12>(immediate) +2;

	cycles += 1 + S_cycles + N_cycles;
}

void branchLink(uint16_t opcode){
	int HLOffset = (opcode >> 11) & 1;
	int immediate = (opcode & 0x7FF);
	if (!HLOffset)
		r[LR] = (signExtend<11>(immediate) << 12) + r[PC] + 2;
	else{
		int nextInstruction = r[PC] + 1;
		r[TRegisters::EProgramCounter] = r[LR] + (immediate << 1);
		r[LR] = nextInstruction | 1;
	}
	if (!HLOffset)
		cycles += S_cycles;
	else{
		cycles += S_cycles + S_cycles + N_cycles;
	}
}

void thumbExecute(uint16_t opcode){
	int subType;
	int instruction;
	cycles += 1;
	__int16 type = (opcode & 0xE000) >> 13;

	switch (type) {
	case 0: //shifts or add or sub, maybe sign extended for immidiates?
		instruction = (opcode >> 11) & 3;
		switch (instruction){
		case 0x00: case 0x01: case 0x02: //shifts
			moveShiftedRegister(opcode);
			break;

		case 0x03: //add / substract
			addSubFunction(opcode);
			break;
		}
		break;

	case 1: // move|compare|substract|add immediate
		movCompSubAddImm(opcode);
		break;

	case 2: //logical ops / memory load / store
		subType = (opcode >> 10) & 7;
		switch (subType){
		case 0: //logical ops reg - reg
			aluOps(opcode);
			break;

		case 1: //high low reg loading, branch
			hiRegOperations(opcode);
			break;

		case 2: case 3: //PC relative load
			PCRelativeLoad(opcode);
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
			loadSPRelative(opcode);
			break;
		}
		break;

	case 5:
		subType = (opcode >> 12) & 0x01;
		switch (subType){
		case 0x00: // load address to reg
			loadAddress(opcode);
			break;

		case 0x01:
			int subType2 = (opcode >> 10) & 1;
			switch (subType2){
			case 0: // add Stack pointer offset
				addOffsetToSP(opcode);
				break;

			case 1: //push pop reg
				pushpop(opcode);
				break;
			}
			break;
		}
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
			branchLink(opcode);
			break;
		}
		break;
	}
}

