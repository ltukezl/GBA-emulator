#include <iostream>
#include <string>
#include "GBAcpu.h"
#include "MemoryOps.h"
#include "interrupt.h"
#include "Constants.h"
#include "ThumbOpCodes.h"

uint32_t highestBit(uint32_t v){
	union { unsigned int u[2]; double d; } t; // temp

	t.u[0] = 0x43300000;
	t.u[1] = v;
	t.d -= 4503599627370496.0;
	return (t.u[1] >> 20) - 0x3FF;
}

void negative(int result)
{
	cpsr.negative = result < 0;
}

void zero(int result)
{
	cpsr.zero = result == 0;
}

void addCarry(int operand1, int operand2, int result)
{
	cpsr.carry = ((operand1 & operand2) | (operand1 & ~result) | (operand2 & ~result)) >> 31 & 1;
}

void addOverflow(int operand1, int operand2, int result)
{
	cpsr.overflow = ((operand1 & operand2 & ~result) | (~operand1 & ~operand2 & result)) >> 31 & 1;
}

void subCarry(int operand1, int operand2, int result)
{
	cpsr.carry = ((operand1 & ~operand2) | (operand1 & ~result) | (~operand2 & ~result)) >> 31 & 1;
}

void subOverflow(int operand1, int operand2, int result)
{
	cpsr.overflow = ((~operand1 & operand2 & result) | (operand1 & ~operand2 & ~result)) >> 31 & 1;
}

//-------------------------------------------------------------------------------------------------------
//last bit out is carry, set carry bits
void lsl(int &saveTo, int from, int immidiate) {
	if (immidiate > 0)
		cpsr.carry = ((unsigned)from >> (32 - immidiate) & 1);
	if (immidiate > 31){
		saveTo = 0;
		zero(saveTo);
	}
	else{
		saveTo = from << immidiate;
		zero(saveTo);
		negative(saveTo);
	}
}
void lsr(int &saveTo, int from, int immidiate) {
	if (immidiate != 0)
		cpsr.carry = ((unsigned)from >> (immidiate - 1) & 1);
	if (immidiate > 31){
		saveTo = 0;
		zero(saveTo);
	}
	else{
		saveTo = (unsigned)from >> immidiate;
		zero(saveTo);
		negative(saveTo);
	}
}

void asr(int &saveTo, int from, int immidiate) {
	if (immidiate != 0)
		cpsr.carry = (from >> ((int)immidiate - 1) & 1);
	if (immidiate > 31 && from & 0x80000000){
		saveTo = 0xFFFFFFFF;
		negative(saveTo);
		cpsr.carry = 1;
	}
	else if (immidiate > 31)
	{
		saveTo = 0;
		zero(saveTo);
		cpsr.carry = 0;
	}
	else{
		saveTo = from >> immidiate;
		zero(saveTo);
		negative(saveTo);
	}
}
//--------------------------------------------------------
void add(int &saveTo, int from, int immidiate) {
	saveTo = from + immidiate;
	negative(saveTo);
	zero(saveTo);
	addCarry(from, immidiate, saveTo);
	addOverflow(from, immidiate, saveTo);
}

void sub(int &saveTo, int from, int immidiate) {
	saveTo = from - immidiate;
	negative(saveTo);
	zero(saveTo);
	subCarry(from, immidiate, saveTo);
	subOverflow(from, immidiate, saveTo);
}
//--------------------------------------------------------
void mov(int &saveTo, int source){
	saveTo = source;
	negative(saveTo);
	zero(saveTo);
}

void cmp(int &saveTo, int source){
	int result = saveTo - source;
	negative(result);
	zero(result);
	subCarry(saveTo, source, result);
	subOverflow(saveTo, source, result);
}

void add8imm(int &saveTo, int immidiate){
	int tmpOperand = saveTo;
	saveTo = saveTo + immidiate;
	negative(saveTo);
	zero(saveTo);
	addCarry(tmpOperand, immidiate, saveTo);
	addOverflow(tmpOperand, immidiate, saveTo);
}

void sub8imm(int &saveTo, int immidiate){
	int tmpOperand = saveTo;
	saveTo = saveTo - immidiate;
	negative(saveTo);
	zero(saveTo);
	subCarry(tmpOperand, immidiate, saveTo);
	subOverflow(tmpOperand, immidiate, saveTo);
}

//--------------------------------------------------------

void TAND(int &saveTo, int immidiate){
	saveTo = saveTo & immidiate;
	negative(saveTo);
	zero(saveTo);
}

void TEOR(int &saveTo, int immidiate){
	saveTo = saveTo ^ immidiate;
	negative(saveTo);
	zero(saveTo);
}

void lslip(int &saveTo, int immidiate){
	lsl(saveTo, saveTo, immidiate);
}

void lsrip(int &saveTo, int immidiate){
	lsr(saveTo, saveTo, immidiate);
}

void asrip(int &saveTo, int immidiate){
	asr(saveTo, saveTo, immidiate);
}

void adc(int &saveTo, int immidiate){
	int tmpOperand = saveTo;
    saveTo = saveTo + immidiate + cpsr.carry;
	zero(saveTo);
	negative(saveTo);
	addCarry(tmpOperand, immidiate, saveTo);
	addOverflow(tmpOperand, immidiate, saveTo);
}

void sbc(int &saveTo, int immidiate){
	int tmpOperand = saveTo;
	saveTo = (saveTo - immidiate) - !cpsr.carry;
	zero(saveTo);
	negative(saveTo);
	subCarry(tmpOperand, immidiate, saveTo);
	subOverflow(tmpOperand, immidiate, saveTo);
}

void rorIP(int &saveTo, int immidiate){ 
	if (immidiate > 32){
		cpsr.carry = 0;
		rorIP(saveTo, immidiate - 32);
	}
	else if (immidiate == 1){
		uint32_t tmp = saveTo;
		saveTo = tmp >> 1 | cpsr.carry << 31;
		cpsr.carry = tmp & 1;
		negative(saveTo);
		zero(saveTo);
	}
	else{
		if (immidiate != 0)
			cpsr.carry = (saveTo >> (immidiate - 1) & 1);
		saveTo = (saveTo >> immidiate) | (saveTo << (32 - immidiate));
		negative(saveTo);
		zero(saveTo);
	}
}

void tst(int &operand1, int operand2){
	int result = operand1 & operand2;
	negative(result);
	zero(result);
}

void neg(int &saveTo, int immidiate){
	saveTo = -immidiate;
	negative(saveTo);
	zero(saveTo);
	subCarry(0, immidiate, saveTo);
	subOverflow(0, immidiate, saveTo);
}

void cmpReg(int &reg1, int reg2){
	cmp(reg1, reg2);
}

void cmnReg(int &reg1, int reg2){
	int result = reg1 + reg2;
	zero(result);
	negative(result);
	addCarry(reg1, reg2, result);
	addOverflow(reg1, reg2, result);
}

void ORR(int &saveTo, int immidiate){
	saveTo = saveTo | immidiate;
	negative(saveTo);
	zero(saveTo);
}

void mul(int &saveTo, int immidiate){
	saveTo = (saveTo * immidiate) & 0xFFFFFFFF;
	negative(saveTo);
	zero(saveTo);
}

void bic(int &saveTo, int immidiate){
	saveTo = saveTo & ~immidiate;
	negative(saveTo);
	zero(saveTo);
}

void mvn(int &saveTo, int immidiate) {
	saveTo = ~immidiate;
	negative(saveTo);
	zero(saveTo);
}

//--------------------------------------------------------
void addNoCond(int& saveTo, int immidiate){
	saveTo = saveTo + immidiate;
}

void movNoCond(int& saveTo, int immidiate){
	saveTo = immidiate;
}

void cmpHL(int& saveTo, int immidiate){
	cmpReg(saveTo, immidiate);
}

void bx(int& saveTo, int immidiate){
	*r[PC] = immidiate & ~ 1;
	cpsr.thumb = immidiate & 1;
}

//--------------------------------------------------------

int BEQ(){
	return cpsr.zero;
}

int BNE(){
	return !cpsr.zero;
}

int BCS(){
	return cpsr.carry;
}

int BCC(){
	return !cpsr.carry;
}

int BMI(){
	return cpsr.negative;
}

int BPL(){
	return !cpsr.negative;
}

int BVS(){
	return cpsr.overflow;
}

int BVC(){
	return !cpsr.overflow;
}

int BHI(){
	return BCS() & BNE();
}

int BLS(){
	return BCC() | BEQ();
}

int BLT(){
	return BMI() ^ BVS();
}

int BGE(){
	return ((BMI() & BVS()) | (BVC() & BPL()));
}

int BGT(){
	return BNE() & BGE();
}

int BLE(){
	return BEQ() | BLT();
}

//-----------------

void(*shifts[3])(int&, int, int) = { lsl, lsr, asr };
void(*arith[2])(int&, int, int) = { add, sub };
void(*movCompIpaddIpsub[4])(int&, int) = { mov, cmp, add8imm, sub8imm };
void(*logicalOps[16])(int&, int) = { TAND, TEOR, lslip, lsrip, asrip, adc, sbc, rorIP, tst, neg, cmpReg, cmnReg, ORR, mul, bic, mvn };
void(*hlOps[4])(int&, int) = { addNoCond, cmpHL, movNoCond, bx };
int(*conditions[14])() = { BEQ, BNE, BCS, BCC, BMI, BPL, BVS, BVC, BHI, BLS, BGE, BLT, BGT, BLE };

char* shifts_s[3] = { "lsl", "lsr", "asr" };
char* arith_s[2] = { "add", "sub" };
char* movCompIpaddIpsub_s[4] = { "mov", "cmp", "add", "sub" };
char* logicalOps_s[16] = { "and", "xor", "lsl", "lsr", "asr", "adc", "sbc", "ror", "tst", "neg", "cmp", "cmn", "or", "mul", "bic", "mvn" };
char* hlOps_s[4] = { "add", "cmp", "mov", "bx" };
char* conditions_s[14] = { "beq", "bne", "bcs", "bcc", "bmi", "bpl", "bvs", "bvc", "bhi", "bls", "bge", "blt", "bgt", "ble" };

void moveShiftedRegister(uint16_t opcode){
	union moveShiftedRegisterOp op = { opcode };
	if (op.immediate == 0 && (op.instruction == 1 || op.instruction == 2))
		shifts[op.instruction](*r[op.destination], *r[op.source], 32);
	else
		shifts[op.instruction](*r[op.destination], *r[op.source], op.immediate);

	cycles += Wait0_S_cycles;
	if (debug)
		std::cout << shifts_s[op.instruction] << " r" << op.destination << " r" << op.source << " " << op.immediate << " ";
}

void addSubFunction(uint16_t opcode){
	union addSubRegisterOp op = { opcode };
	int value = op.useImmediate ? op.regOrImmediate : *r[op.regOrImmediate];
	arith[op.Sub](*r[op.destination], *r[op.source], value);

	cycles += Wait0_S_cycles;
	if (debug)
		std::cout << arith_s[op.Sub] << " r" << op.destination << " r" << op.source << (op.useImmediate ? " " : " r") << op.regOrImmediate << " ";
}

void movCompSubAddImm(uint16_t opcode){
	union movCmpAddSub op = { opcode };
	movCompIpaddIpsub[op.instruction](*r[op.destination], op.offset);

	cycles += Wait0_S_cycles;
	if (debug)
		std::cout << movCompIpaddIpsub_s[op.instruction] << " r" << op.destination << " #" << op.offset << " ";
}

void aluOps(uint16_t opcode){
	union aluOps op = { opcode };
	logicalOps[op.instruction](*r[op.destination], *r[op.source]);

	cycles += Wait0_S_cycles;
	if (op.instruction == 2 || op.instruction == 3 || op.instruction == 4 || op.instruction == 12)
		cycles += 1;

	if (debug)
		std::cout << logicalOps_s[op.instruction]  << " r" << op.destination << " r" << op.source << " ";
}

void hiRegOperations(uint16_t opcode){
	union hiRegOps op = { opcode };
	uint8_t newDestinationReg = 8 * op.destHiBit + op.destination;
	uint32_t operand = *r[op.source];

	if (newDestinationReg == 15){
		*r[newDestinationReg] += 2;
		operand &= ~1;
	}

	if (op.source == PC){
		operand += 2;
		operand &= ~1;
	}

	hlOps[op.instruction](*r[newDestinationReg], operand);

	cycles += Wait0_S_cycles;

	if ((op.source == 15) | (newDestinationReg == 3))
		cycles += Wait0_N_cycles + 1;

	if (debug)
		std::cout << std::dec << hlOps_s[op.instruction] << " r" << (newDestinationReg) << " r" << op.source << std::hex;
}

void PCRelativeLoad(uint16_t opcode){
	union PCRelativeLoad op = { opcode };
	uint32_t tmpPC = (*r[PC] + 2) & ~2;
	tmpPC += (op.offset << 2);
	*r[op.destination] = loadFromAddress32(tmpPC);

	cycles += 1;

	if (debug)
		std::cout << "ldr r" << op.destination << ", =" << std::hex << loadFromAddress32(tmpPC, true) << std::dec << " ";
}

void loadStoreRegOffset(uint16_t opcode){
	union loadStoreRegOffset op = { opcode };
	uint32_t totalAddress = *r[op.baseReg] + *r[op.offsetReg];

	if (op.loadBit && op.byteSize)
		*r[op.destSourceReg] = loadFromAddress(totalAddress);
	else if (op.loadBit && !op.byteSize)
		*r[op.destSourceReg] = loadFromAddress32(totalAddress);
	else if (!op.loadBit && op.byteSize)
		writeToAddress(totalAddress, *r[op.destSourceReg]);
	else
		writeToAddress32(totalAddress, *r[op.destSourceReg]);

	cycles += 1;
	if (!op.loadBit)
		cycles += Wait0_N_cycles + 1; 

	if (debug){
		if (op.loadBit && op.byteSize)
			std::cout << "strb r";
		else if (op.loadBit && !op.byteSize)
			std::cout << "str r";
		else if (!op.loadBit && op.byteSize)
			std::cout << "ldrb r";
		else
			std::cout << "ldr r";
		std::cout << op.destSourceReg << " [r" << op.baseReg << ", r" << op.offsetReg << " ] ";
	}
}

void loadStoreSignExtend(uint16_t opcode){
	union loadStoreSignExtended op = { opcode };
	uint32_t totalAddress = *r[op.baseReg] + *r[op.offsetReg];

	if (op.halfWord && op.extend){
		if (totalAddress & 1) //misaligned address
			*r[op.destSourceReg] = signExtend<8>(loadFromAddress16(totalAddress));
		else
			*r[op.destSourceReg] = signExtend<16>(loadFromAddress16(totalAddress));
	}
	else if (op.halfWord && !op.extend)	
		*r[op.destSourceReg] = loadFromAddress16(totalAddress);
	else if (!op.halfWord && op.extend)
		*r[op.destSourceReg] = signExtend<8>(loadFromAddress(totalAddress));
	else
		writeToAddress16(totalAddress, *r[op.destSourceReg]);

	cycles += 1;
	if (!op.halfWord && !op.extend)
		cycles += Wait0_N_cycles + 1;

	if (debug){
		if (!op.halfWord && !op.extend)
			std::cout << "strh r";
		if (!op.halfWord && op.extend)
			std::cout << "ldrh r";
		if (op.halfWord && !op.extend)
			std::cout << "ldsb r";
		if (op.halfWord && op.extend)
			std::cout << "ldsh r";
		std::cout << op.destSourceReg << ", [r" << op.baseReg << ",r" << op.offsetReg << "] ";
	}
}

void loadStoreImm(uint16_t opcode){
	union loadStoreImmediate op = { opcode };
	uint8_t immediate = op.offset << (2 * !op.byteSize);
	uint32_t totalAddress = *r[op.baseReg] + immediate;

	if (op.loadFlag && op.byteSize)
		*r[op.destSourceReg] = loadFromAddress(totalAddress);
	else if (op.loadFlag && !op.byteSize)
		*r[op.destSourceReg] = loadFromAddress32(totalAddress);
	else if (!op.loadFlag && op.byteSize)
		writeToAddress(totalAddress, *r[op.destSourceReg]);
	else
		writeToAddress32(totalAddress, *r[op.destSourceReg]);

	cycles += 1;
	if (!op.loadFlag)
		cycles += Wait0_N_cycles + 1;

	if (debug && op.loadFlag)
		std::cout << "ldr r" << op.destSourceReg << " [r" << op.baseReg << " " << immediate << "] ";
	else if (debug && !op.loadFlag)
		std::cout << "str [r" << op.baseReg << " " << immediate << "] r" << op.destSourceReg << " ";
}

void loadStoreHalfword(uint16_t opcode){
	union loadStoreHalfWord op = { opcode };
	int immediate = op.offset << 1; //half word alignment, 5 bits to 6 bits last bit is 0
	if (op.loadFlag)
		*r[op.destSourceReg] = loadFromAddress16(*r[op.baseReg] + immediate);
	else
		writeToAddress16(*r[op.baseReg] + immediate, *r[op.destSourceReg]);

	cycles += 1;
	if (!op.loadFlag)
		cycles += Wait0_N_cycles + 1;

	if (debug && op.loadFlag)
		std::cout << "ldrh r" << op.destSourceReg << " [r" << op.baseReg << " " << immediate << "] ";
	else if (debug && !op.loadFlag)
		std::cout << "strh [r" << op.baseReg << " " << immediate << "] r" << op.destSourceReg << " ";
}

void loadSPRelative(uint16_t opcode){
	union SPrelativeLoad op = { opcode };
	if (op.loadFlag)
		*r[op.destSourceReg] = loadFromAddress32(*r[SP] + (op.immediate << 2));
	else
		writeToAddress32(*r[SP] + (op.immediate << 2), *r[op.destSourceReg]);

	cycles += 1;
	if (!op.loadFlag)
		cycles += Wait0_N_cycles + 1;

	if (debug && op.loadFlag)
		std::cout << "ldr r" << op.destSourceReg << ", [sp " << op.immediate << 2 << "] ";
	else if (debug && !op.loadFlag)
		std::cout << "str [sp " << op.immediate << 2 << "],  r" << op.destSourceReg << " ";
}


void loadAddress(uint16_t opcode){
	union loadAddress op = { opcode };
	int rs = op.useSP ? *r[SP] : ((*r[PC] + 2) & ~2);
	*r[op.destination] = (op.immediate << 2) + rs;

	cycles += Wait0_S_cycles;
	if (debug && op.useSP)
		std::cout << "add r" << op.destination << ", SP, 0x" << (op.immediate << 2) << " ";
	if (debug && !op.useSP)
		std::cout << "add r" << op.destination << ", PC, 0x" << (op.immediate << 2) << " ";
}

void addOffsetToSP(uint16_t opcode){
	int loadFlag = (opcode >> 7) & 1;
	int immediate = (opcode & 0x7F) << 2;
	*r[SP] += loadFlag ? -immediate : immediate;

	cycles += Wait0_S_cycles;

	if (debug && loadFlag)
		std::cout << "sub sp, 0x" << immediate << " ";
	else if (debug && !loadFlag)
		std::cout << "add sp, 0x" << immediate << " ";
}

void pushpop(uint16_t opcode){
	union pushPopReg op = { opcode };
	int immediate = op.regList;

	if (op.loadBit){
		for (int i = 0; i < 8; i++){
			if (immediate & 1){
				*r[i] = POP();
			}
			immediate = immediate >> 1;
		}
		*r[PC] = op.PCRLBit ? (POP() & -2) : (*r[PC]);
		
		if (immediate & 0x80)
			cycles += 1;
		
		if (debug)
			std::cout << "pop ";
	}

	else{
		if (op.PCRLBit)
			PUSH(*r[LR]);
		for (int i = 0; i < 8; i++){
			if (immediate & 0x80)
				PUSH(*r[7-i]);
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

	bool rInList = false;
	bool first = true;
	uint32_t oldAddr = 0;

	if (loadFlag){
		if (immediate == 0){
			*r[PC] = (loadFromAddress32(*r[baseReg])) & ~1;
			*r[baseReg] += 0x40;
		}
		else {
			for (int i = 0; i < 8; i++){
				if (immediate & 1){
					*r[i] = loadFromAddress32(*r[baseReg]);
					*r[baseReg] += 4;
				}
				immediate >>= 1;
			}
		}
		if (debug)
			std::cout << "ldmia ";
	}
	else{
		if (immediate == 0){
			writeToAddress32(*r[baseReg], *r[PC] + 4);
			*r[baseReg] += 0x40;
		}
		else{
			for (int i = 0; i < 8; i++){
				if (immediate & 1){
					if (i == baseReg && !first){
						rInList = true;
						oldAddr = *r[baseReg];
						continue;
					}
					first = false;
					writeToAddress32(*r[baseReg], *r[i]);
					*r[baseReg] += 4;
					
				}
				immediate >>= 1;
			}
		}
		if (rInList){
			writeToAddress32(oldAddr, *r[baseReg]);
		}
		if (debug)
			std::cout << "stmia ";
	}
	cycles += 1;
}

void conditionalBranch(uint16_t opcode){
	union conditionalBranchOp op = { opcode };
	*r[PC] += conditions[op.condition]() ? ((op.immediate << 1) + 2) : 0;

	cycles += Wait0_S_cycles;
	if (conditions[op.condition]())
		cycles += Wait0_S_cycles + Wait0_N_cycles;

	if (debug)
		std::cout << conditions_s[op.condition] << " " << conditions[op.condition]() << " ";
}

void unconditionalBranch(uint16_t opcode){
	int immediate = (opcode & 0x7FF) << 1;
	*r[PC] += signExtend<12>(immediate) + 2;

	cycles += 1 + Wait0_S_cycles + Wait0_N_cycles;

	if (debug)
		std::cout << "b " << std::hex << *r[PC] << std::dec << " ";
}

void branchLink(uint16_t opcode){
	int HLOffset = (opcode >> 11) & 1;
	int immediate = (opcode & 0x7FF);
	if (!HLOffset)
		*r[LR] = (signExtend<11>(immediate) << 12) + *r[PC] + 2;
	else{
		int nextInstruction = *r[PC] + 1;
		*r[PC] = *r[LR] + (immediate << 1);
		*r[LR] = nextInstruction | 1;
	}
	if (!HLOffset)
		cycles += Wait0_S_cycles;
	else{
		cycles += Wait0_S_cycles + Wait0_S_cycles + Wait0_N_cycles;
		if (debug)
			std::cout << "BL " << *r[PC] << " ";
	}
}

int thumbExecute(uint16_t opcode){
	int subType;
    int instruction;
	*r[PC] += 2;
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
						//interruptController();	
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
    return 0;
}

