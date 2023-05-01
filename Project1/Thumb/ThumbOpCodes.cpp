#include <iostream>
#include <string>
#include "GBAcpu.h"
#include "Memory/MemoryOps.h"
#include "Interrupt/interrupt.h"
#include "Constants.h"
#include "ThumbOpCodes.h"
#include "Arm/armopcodes.h"
#include "CommonOperations/conditions.h"
#include "CommonOperations/arithmeticOps.h"
#include "CommonOperations/logicalOps.h"

void mul(int &saveTo, int immidiate, int immidiate2){
	saveTo = (immidiate2 * immidiate) & 0xFFFFFFFF;
	negative(saveTo);
	zero(saveTo);
}

void bx(int& saveTo, int immidiate, int immidiate2){
	r[TRegisters::EProgramCounter] = immidiate2 & ~1;
	cpsr.thumb = immidiate2 & 1;
}

void(*shifts[3])(int&, int, int) = { lslCond, lsrCond, asrCond };
void(*arith[2])(int&, int, int) = { Adds, Subs };
void(*movCompIpaddIpsub[4])(int&, int, int) = { Movs, Cmp, Adds, Subs };
void(*logicalOps[16])(int&, int, int) = { Ands, Eors, lslCond, lsrCond, asrCond, Adcs, Sbcs, rorCond, Tst, Neg, Cmp, Cmn, Orrs, mul, Bics, Mvns };
void(*hlOps[4])(int&, int, int) = { Add, Cmp, Mov, bx };

std::string shifts_s[3] = { "lsl", "lsr", "asr" };
std::string arith_s[2] = { "add", "sub" };
std::string movCompIpaddIpsub_s[4] = { "mov", "cmp", "add", "sub" };
std::string logicalOps_s[16] = { "and", "xor", "lsl", "lsr", "asr", "adc", "sbc", "ror", "tst", "neg", "cmp", "cmn", "or", "mul", "bic", "mvn" };
std::string hlOps_s[4] = { "add", "cmp", "mov", "bx" };

void moveShiftedRegister(uint16_t opcode){
	union moveShiftedRegisterOp op = { opcode };
	if (op.immediate == 0 && (op.instruction == 1 || op.instruction == 2))
		shifts[op.instruction]((int&)r[op.destination], r[op.source], 32);
	else
		shifts[op.instruction]((int&)r[op.destination], r[op.source], op.immediate);

	cycles += S_cycles;
	if (debug)
		std::cout << shifts_s[op.instruction] << " r" << +op.destination << " r" << +op.source << " " << +op.immediate << " ";
}

void addSubFunction(uint16_t opcode){
	union addSubRegisterOp op = { opcode };
	int value = op.useImmediate ? op.regOrImmediate : r[op.regOrImmediate];
	arith[op.Sub]((int&)r[op.destination], r[op.source], value);

	cycles += S_cycles;
	if (debug)
		std::cout << arith_s[op.Sub] << " r" << +op.destination << " r" << +op.source << (op.useImmediate ? " " : " r") << +op.regOrImmediate << " ";
}

void movCompSubAddImm(uint16_t opcode){
	union movCmpAddSub op = { opcode };
	movCompIpaddIpsub[op.instruction]((int&)r[op.destination], r[op.destination], op.offset);

	cycles += S_cycles;
	if (debug)
		std::cout << movCompIpaddIpsub_s[op.instruction] << " r" << +op.destination << " #" << +op.offset << " ";
}

void aluOps(uint16_t opcode){
	union aluOps op = { opcode };
	logicalOps[op.instruction]((int&)r[op.destination], r[op.destination], r[op.source]);

	cycles += S_cycles;
	if (op.instruction == 2 || op.instruction == 3 || op.instruction == 4 || op.instruction == 12)
		cycles += 1;

	if (debug)
		std::cout << logicalOps_s[op.instruction] << " r" << +op.destination << " r" << +op.source << " ";
}

void hiRegOperations(uint16_t opcode){

	union hiRegOps op = { opcode };
	uint8_t newDestinationReg = 8 * op.destHiBit + op.destination;
	uint32_t operand1 = r[newDestinationReg];
	uint32_t operand2 = r[op.source];

	if (op.source == PC){
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

	if (debug)
		std::cout << std::dec << hlOps_s[op.instruction] << " r" << +(newDestinationReg) << " r" << +op.source << std::hex;
}

void PCRelativeLoad(uint16_t opcode){
	union PCRelativeLoad op = { opcode };
	uint32_t tmpPC = (r[PC] + 2) & ~2;
	tmpPC += (op.offset << 2);
	r[op.destination] = loadFromAddress32(tmpPC);

	cycles += 1;

	if (debug)
		std::cout << "ldr r" << +op.destination << ", =" << std::hex << loadFromAddress32(tmpPC, true) << std::dec << " ";
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

	if (debug){
		if (op.loadBit && op.byteSize)
			std::cout << "strb r";
		else if (op.loadBit && !op.byteSize)
			std::cout << "str r";
		else if (!op.loadBit && op.byteSize)
			std::cout << "ldrb r";
		else
			std::cout << "ldr r";
		std::cout << +op.destSourceReg << " [r" << +op.baseReg << ", r" << +op.offsetReg << " ] ";
	}
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

	if (debug){
		if (!op.halfWord && !op.extend)
			std::cout << "strh r";
		if (!op.halfWord && op.extend)
			std::cout << "ldrh r";
		if (op.halfWord && !op.extend)
			std::cout << "ldsb r";
		if (op.halfWord && op.extend)
			std::cout << "ldsh r";
		std::cout << +op.destSourceReg << ", [r" << +op.baseReg << ",r" << +op.offsetReg << "] ";
	}
}

void loadStoreImm(uint16_t opcode){
	union loadStoreImmediate op = { opcode };
	uint8_t immediate = op.offset << (2 * !op.byteSize);
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

	if (debug && op.loadFlag)
		std::cout << "ldr r" << +op.destSourceReg << " [r" << +op.baseReg << " " << +immediate << "] ";
	else if (debug && !op.loadFlag)
		std::cout << "str [r" << +op.baseReg << " " << +immediate << "] r" << +op.destSourceReg << " ";
}

void loadStoreHalfword(uint16_t opcode){
	union loadStoreHalfWord op = { opcode };
	int immediate = op.offset << 1; //half word alignment, 5 bits to 6 bits last bit is 0
	if (op.loadFlag)
		r[op.destSourceReg] = loadFromAddress16(r[op.baseReg] + immediate);
	else
		writeToAddress16(r[op.baseReg] + immediate, r[op.destSourceReg]);

	cycles += S_cycles + N_cycles + 1;

	if (debug && op.loadFlag)
		std::cout << "ldrh r" << +op.destSourceReg << " [r" << +op.baseReg << " " << +immediate << "] ";
	else if (debug && !op.loadFlag)
		std::cout << "strh [r" <<+ op.baseReg << " " << +immediate << "] r" << +op.destSourceReg << " ";
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

	if (debug && op.loadFlag)
		std::cout << "ldr r" << +op.destSourceReg << ", [sp " << +op.immediate << 2 << "] ";
	else if (debug && !op.loadFlag)
		std::cout << "str [sp " << +op.immediate << 2 << "],  r" << +op.destSourceReg << " ";
}


void loadAddress(uint16_t opcode){
	union loadAddress op = { opcode };
	int rs = op.useSP ? r[TRegisters::EStackPointer] : ((r[TRegisters::EProgramCounter] + 2) & ~2);
	r[op.destination] = (op.immediate << 2) + rs;

	cycles += S_cycles;
	if (debug && op.useSP)
		std::cout << "add r" << +op.destination << ", SP, 0x" << +(op.immediate << 2) << " ";
	if (debug && !op.useSP)
		std::cout << "add r" << +op.destination << ", PC, 0x" << +(op.immediate << 2) << " ";
}

void addOffsetToSP(uint16_t opcode){
	int loadFlag = (opcode >> 7) & 1;
	int immediate = (opcode & 0x7F) << 2;
	r[SP] += loadFlag ? -immediate : immediate;

	cycles += S_cycles;

	if (debug && loadFlag)
		std::cout << "sub sp, 0x" << +immediate << " ";
	else if (debug && !loadFlag)
		std::cout << "add sp, 0x" << +immediate << " ";
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

		if (debug)
			std::cout << "pop ";
	}

	else{
		if (op.PCRLBit)
			PUSH(r[LR]);
		for (int i = 0; i < 8; i++){
			if (immediate & 0x80)
				PUSH(r[7 - i]);
			immediate = immediate << 1;
		}
		if (debug)
			std::cout << "push ";
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
		if (debug)
			std::cout << "ldmia ";
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
		if (debug)
			std::cout << "stmia ";
	}
	cycles += 1;
}

void conditionalBranch(uint16_t opcode){
	union conditionalBranchOp op = { opcode };
	r[PC] += conditions[op.condition]() ? ((op.immediate << 1) + 2) : 0;

	cycles += S_cycles;
	if (conditions[op.condition]())
		cycles += S_cycles + N_cycles;

	if (debug)
		std::cout << conditions_s[op.condition] << " " << conditions[op.condition]() << " ";
}

void unconditionalBranch(uint16_t opcode){
	int immediate = (opcode & 0x7FF) << 1;
	r[PC] += signExtend<12>(immediate) +2;

	cycles += 1 + S_cycles + N_cycles;

	if (debug)
		std::cout << "b " << std::hex << r[PC] << std::dec << " ";
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
		if (debug)
			std::cout << "BL " << r[PC] << " ";
	}
}

void thumbExecute(uint16_t opcode){
	int subType;
	int instruction;
	r[TRegisters::EProgramCounter] += 2;
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

