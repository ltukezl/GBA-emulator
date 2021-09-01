#include <iostream>
#include <string>
#include "GBAcpu.h"
#include "MemoryOps.h"
#include "interrupt.h"
#include "Constants.h"
#include "ThumbOpCodes.h"

#define SETBIT(REG, POS) (REG |= (1 << POS))
#define ZEROBIT(REG, POS) (REG &= (~(1<< POS)))

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
	saveTo = (saveTo - immidiate) - cpsr.carry;
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
		saveTo = (saveTo << immidiate) | (saveTo >> (32 - immidiate));
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

void moveShiftedRegister(int opcode){
	union moveShiftedRegisterOp op;
	op.op = opcode;
	if (op.immediate == 0 && (op.instruction == 1 || op.instruction == 2))
		shifts[op.instruction](*r[op.destination], *r[op.source], 32);
	else
		shifts[op.instruction](*r[op.destination], *r[op.source], op.immediate);

	cycles += Wait0_S_cycles;
	if (debug)
		std::cout << shifts_s[op.instruction] << " r" << op.destination << " r" << op.source << " " << op.immediate << " ";
}

void addSubFunction(int opcode){
	int rd = opcode & 0x7; //register, destination
	int rs = (opcode >> 3) & 0x7; //register, source
	int operation = (opcode >> 9) & 1;
	int immediateFlag = (opcode >> 10) & 1;
	int immediate = (opcode >> 6) & 7;
	int value = immediateFlag ? immediate : *r[immediate];
	arith[operation](*r[rd], *r[rs], value);

	cycles += Wait0_S_cycles;
	if (debug)
		std::cout << arith_s[operation] << " r" << rd << " r" << rs << (immediateFlag ? " " : " r") << immediate << " ";
}

void movCompSubAddImm(int opcode){
	int instruction = (opcode >> 11) & 3;
	int rd = (opcode >> 8) & 7;
	int immediate = (opcode & 0xFF);
	movCompIpaddIpsub[instruction](*r[rd], immediate);

	cycles += Wait0_S_cycles;
	if (debug)
		std::cout << movCompIpaddIpsub_s[instruction] << " r" << rd  << " #" << immediate << " ";
}

void aluOps(int opcode){
	int instruction = (opcode >> 6) & 0xF;
	int rd = opcode & 7; //register, destination
	int rs = (opcode >> 3) & 7; //register, source
	logicalOps[instruction](*r[rd], *r[rs]);

	cycles += Wait0_S_cycles;
	if (instruction == 2 || instruction == 3 || instruction == 4 || instruction == 12)
		cycles += 1;

	if (debug)
		std::cout << logicalOps_s[instruction] << " r" << rd << " r" << rd << " r" << rs << " ";
}

void hiRegOperations(int opcode){
	int instruction = (opcode >> 8) & 3;
	int rd = opcode & 0x07; //register, destination
	int rs = (opcode >> 3) & 0xF; //register, source, exceptionally 4 bits as this opcode can access r0-r15
	int hi1 = ((opcode >> 4) & 8); //high reg flags enables access to r8-r15 registers
	if ((rd | hi1) == 15)
		hlOps[instruction](*r[rd | hi1], (rs == 15) ? ((*r[PC] + 2) & ~1) : *r[rs] & ~1);
	else
		hlOps[instruction](*r[rd | hi1], (rs == 15) ? ((*r[PC] + 2) & ~1) : *r[rs]);

	cycles += Wait0_S_cycles;

	if ((rs == 15) | (instruction == 3))
		cycles += Wait0_N_cycles + 1;

	if (debug)
		std::cout << std::dec << hlOps_s[instruction] << " r" << (rd | hi1) << " r" << rs << std::hex;
}

void PCRelativeLoad(int opcode){
	//check this out later, memory masking?, potentially broken.
	int tmpPC = (*r[PC] + 2) & ~2;
	int rs = (opcode >> 8) & 7;
	int immediate = (opcode & 0xFF) << 2; //8 bit value to 10 bit value, last bits are 00 to be word alinged
	tmpPC += immediate;
	*r[rs] = loadFromAddress32(tmpPC);

	cycles += 1;

	if (debug)
		std::cout << "ldr r" << rs << ", =" << loadFromAddress32(tmpPC, true) << " ";
}

void loadStoreRegOffset(int opcode){
	int rd = opcode & 7;
	int rb = (opcode >> 3) & 7; // base reg
	int ro = (opcode >> 6) & 7; // offset reg
	int byteFlag = (opcode >> 10) & 1;
	int loadFlag = (opcode >> 11) & 1;
	if (!loadFlag)
		if (byteFlag)
			writeToAddress(*r[ro] + *r[rb], *r[rd]);
		else
			writeToAddress32(*r[ro] + *r[rb], *r[rd]);
	else
		*r[rd] = byteFlag ? loadFromAddress(*r[rb] + *r[ro]) : loadFromAddress32(*r[rb] + *r[ro]);

	cycles += 1;
	if (!loadFlag)
		cycles += Wait0_N_cycles + 1; 

	if (debug && byteFlag && !loadFlag)
		std::cout << "strb r" << rd << " [" << rb << ", " << ro << " ]";
	else if (debug && !byteFlag && !loadFlag)
		std::cout << "str r" << rd << " [" << rb << ", " << ro << " ]";
	else if (debug && byteFlag && loadFlag)
		std::cout << "ldrb r" << rd << " [" << rb << ", " << ro << " ]";
	else if (debug && !byteFlag && loadFlag)
		std::cout << "ldr r" << rd << " [" << rb << ", " << ro << " ]";
	
}

void loadStoreSignExtend(int opcode){
	int rd = opcode & 7;
	int rb = (opcode >> 3) & 7; // base reg
	int ro = (opcode >> 6) & 7; // offset reg
	int signFlag = (opcode >> 10) & 1;
	int hFlag = ((opcode >> 11) & 1);
	if (!hFlag && !signFlag) //strh
		writeToAddress16(*r[rb] + *r[ro], *r[rd]);
	else if (!signFlag && hFlag) //ldrh
		*r[rd] = loadFromAddress16(*r[rb] + *r[ro]);
	else if (signFlag && !hFlag) //ldsb
		*r[rd] = signExtend<8>(loadFromAddress(*r[rb] + *r[ro]));
	else //ldsh
		*r[rd] = signExtend<16>(loadFromAddress16(*r[ro] + *r[rb]));

	cycles += 1;
	if (!hFlag && !signFlag)
		cycles += Wait0_N_cycles + 1;

	if (debug && !hFlag && !signFlag)
		std::cout << "strh [r" << rb << ", r" << ro << "] ,r" << ro << " ";
	if (debug && !hFlag && signFlag)
		std::cout << "ldrh r" << rd << ", [r" << rb << ",r" << ro << "] ";
	if (debug && hFlag && !signFlag)
		std::cout << "ldsb r" << rd << ", [r" << rb << ",r" << ro << "] ";
	if (debug && hFlag && signFlag)
		std::cout << "ldsh r" << rd << ", [r" << rb << ",r" << ro << "] ";
}

void loadStoreImm(int opcode){
	int loadFlag = (opcode >> 11) & 1;
	int byteFlag = (opcode >> 12) & 1;
	int immediate = byteFlag ? (opcode >> 6) & 0x1F : (opcode >> 4) & 0x7C; //with word access, immediate must be word aligned. thus we move is 2 less and zero 2 lsb
	int rs = (opcode >> 3) & 0x07;
	int rd = opcode & 0x07;
	if (loadFlag)
		*r[rd] = byteFlag ? loadFromAddress(*r[rs] + immediate) : loadFromAddress32(*r[rs] + immediate);
	else
		byteFlag ? writeToAddress(*r[rs] + immediate, *r[rd]) : writeToAddress32(*r[rs] + immediate, *r[rd]);

	cycles += 1;
	if (!loadFlag)
		cycles += Wait0_N_cycles + 1;

	if (debug && loadFlag)
		std::cout << "ldr r" << rd << " [r" << rs << " " << immediate << "] ";
	else if (debug && !loadFlag)
		std::cout << "str [r" << rs << " " << immediate << "] r" << rd << " ";
}

void loadStoreHalfword(int opcode){
	int loadFlag = (opcode >> 11) & 1;
	int immediate = (opcode >> 5) & 0x3E; //half word alignment, 5 bits to 6 bits last bit is 0
	int rs = (opcode >> 3) & 0x07;
	int rd = opcode & 0x07;
	if (loadFlag)
		*r[rd] = loadFromAddress16(*r[rs] + immediate);
	else
		writeToAddress16(*r[rs] + immediate, *r[rd]);

	cycles += 1;
	if (!loadFlag)
		cycles += Wait0_N_cycles + 1;

	if (debug && loadFlag)
		std::cout << "ldrh r" << rd << " [r" << rs << " " << immediate << "] ";
	else if (debug && !loadFlag)
		std::cout << "strh [r" << rs << " " << immediate << "] r" << rd << " ";
}

void loadSPRelative(int opcode){
	int loadFlag = (opcode >> 11) & 1;
	int rd = (opcode >> 8) & 0x07;
	int immediate = (opcode & 0xFF) << 2;
	if (loadFlag)
		*r[rd] = loadFromAddress32(*r[SP] + immediate);
	else
		writeToAddress32(*r[SP] + immediate, *r[rd]);

	cycles += 1;
	if (!loadFlag)
		cycles += Wait0_N_cycles + 1;

	if (debug && loadFlag)
		std::cout << "ldr r" << rd << ", [sp " << immediate << "] ";
	else if (debug && !loadFlag)
		std::cout << "str [sp " << immediate << "],  r" << rd << " ";
}


void loadAddress(int opcode){
	int rs = ((opcode >> 11) & 1) ? *r[SP] : ((*r[PC] & ~2) + 4);
	int rd = (opcode >> 8) & 0x07;
	int immediate = (opcode & 0xFF) << 2;
	*r[rd] = immediate + rs;

	cycles += Wait0_S_cycles;
	if (debug && rs)
		std::cout << "add r" << rd << ", SP, 0x" << immediate << " ";
	if (debug && !rs)
		std::cout << "add r" << rd << ", PC, 0x" << immediate << " ";
}

void addOffsetToSP(int opcode){
	int loadFlag = (opcode >> 7) & 1;
	int immediate = (opcode & 0x7F) << 2;
	*r[SP] += loadFlag ? -immediate : immediate;

	cycles += Wait0_S_cycles;

	if (debug && loadFlag)
		std::cout << "sub sp, 0x" << immediate << " ";
	else if (debug && !loadFlag)
		std::cout << "add sp, 0x" << immediate << " ";
}

void pushpop(int opcode){
	int immediate = opcode & 0xFF;
	int popFlag = (opcode >> 11) & 1;

	if (popFlag){
		for (int i = 0; i < 8; i++){
			if (immediate & 1){
				*r[i] = POP();
			}
			immediate = immediate >> 1;
		}
		*r[PC] = ((opcode >> 8) & 1) ? (POP() & -2) : (*r[PC]);
		
		if (immediate & 0x80)
			cycles += 1;
		
		if (debug)
			std::cout << "pop ";
	}

	else{
		if ((opcode >> 8) & 1)
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

//can be optimized with whiles when needed (or inlines or macro with msb find)
void multiLoad(int opcode){
	int immediate = opcode & 0xFF;
	int loadFlag = (opcode >> 11) & 1;
	int baseReg = (opcode >> 8) & 7;
	if (loadFlag){
		for (int i = 0; i < 8; i++){
			if (immediate & 1){
				*r[i] = loadFromAddress32(*r[baseReg]);
				*r[baseReg] += 4;
			}
			immediate >>= 1;
		}
	}
	else{
		for (int i = 0; i < 8; i++){
			if (immediate & 1){
				writeToAddress32(*r[baseReg], *r[i]);
				*r[baseReg] += 4;
			}
			immediate >>= 1;
		}
	}

	cycles += 1;
	if (debug)
		std::cout << "stmia ";
}

void conditionalBranch(int opcode){
	int immediate = opcode & 0xFF;
	int condition = (opcode >> 8) & 0x0F;
	*r[PC] += conditions[condition]() ? (((__int8)immediate << 1) + 2) : 0;

	cycles += Wait0_S_cycles;
	if (conditions[condition]())
		cycles += Wait0_S_cycles + Wait0_N_cycles;

	if (debug)
		std::cout << conditions_s[condition] << " " << conditions[condition]() << " ";
}

void unconditionalBranch(int opcode){
	int immediate = (opcode & 0x7FF) << 1;
	*r[PC] += signExtend<12>(immediate) + 2;

	cycles += 1 + Wait0_S_cycles + Wait0_N_cycles;

	if (debug)
		std::cout << "b " << std::hex << *r[PC] << std::dec << " ";
}

void branchLink(int opcode){
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

int thumbExecute(__int16 opcode){
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
    return 0;
}

