#include <iostream>
#include "GBAcpu.h"
#include "MemoryOps.h"
#include "interrupt.h"
#include "Constants.h"

#define SETBIT(REG, POS) (REG |= (1 << POS))
#define ZEROBIT(REG, POS) (REG &= (~(1<< POS)))

void negative(int result)
{
	result >> 31 ? SETBIT(cprs, 31) : ZEROBIT(cprs, 31);
}

void zero(int result)
{
	result ? ZEROBIT(cprs, 30) : SETBIT(cprs, 30);
}

void addCarry(int operand1, int operand2, int result)
{
	bool carry = ((operand1 & operand2) | (operand1 & ~result) | (operand2 & ~result)) >> 31;
	carry ? SETBIT(cprs, 29) : ZEROBIT(cprs, 29);
}

void addOverflow(int operand1, int operand2, int result)
{
	bool overflow = ((operand1 & operand2 & ~result) | (~operand1 & ~operand2 & result)) >> 31;
	overflow ? SETBIT(cprs, 28) : ZEROBIT(cprs, 28);
}

void subCarry(int operand1, int operand2, int result)
{
	bool carry = ((operand1 & ~operand2) | (operand1 & ~result) | (~operand2 & ~result)) >> 31;
	carry ? SETBIT(cprs, 29) : ZEROBIT(cprs, 29);
}

void subOverflow(int operand1, int operand2, int result)
{
	bool overflow = ((~operand1 & operand2 & result) | (operand1 & ~operand2 & ~result)) >> 31;
	overflow ? SETBIT(cprs, 28) : ZEROBIT(cprs, 28);
}

//-------------------------------------------------------------------------------------------------------
//last bit out is carry, set carry bits
void lsl(int &saveTo, int from, int immidiate) {
	(saveTo >> (32 - immidiate) & 1) ? SETBIT(cprs, 30) : ZEROBIT(cprs, 30);
	saveTo = from << immidiate;
}
void lsr(int &saveTo, int from, int immidiate) {
	(saveTo >> (immidiate - 1) & 1) ? SETBIT(cprs, 30) : ZEROBIT(cprs, 30);
	saveTo = (unsigned)from >> immidiate;
}

void asr(int &saveTo, int from, int immidiate) {
	(saveTo >> ((int)immidiate - 1) & 1) ? SETBIT(cprs, 30) : ZEROBIT(cprs, 30);
	saveTo = from >> immidiate;
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
    saveTo = saveTo + immidiate + ((cprs >> 29) & 1);
	zero(saveTo);
	negative(saveTo);
	addCarry(tmpOperand, immidiate, saveTo);
	addOverflow(tmpOperand, immidiate, saveTo);
}

void sbc(int &saveTo, int immidiate){
	int tmpOperand = saveTo;
    saveTo = saveTo - immidiate - (~((cprs >> 29) & 1));
	zero(saveTo);
	negative(saveTo);
	subCarry(tmpOperand, immidiate, saveTo);
	subOverflow(tmpOperand, immidiate, saveTo);
}

void rorIP(int &saveTo, int immidiate){
	(saveTo >> (immidiate - 1) & 1) ? SETBIT(cprs, 30) : ZEROBIT(cprs, 30);
	saveTo = (saveTo << immidiate) | (saveTo >> (32 - immidiate));
	negative(saveTo);
	zero(saveTo);
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
	bool thumb = immidiate & 1;
	thumb ? SETBIT(cprs, 5) : ZEROBIT(cprs, 5);
}

//--------------------------------------------------------

int BEQ(){
	return (cprs >> 30) & 1;
}

int BNE(){
	return (~cprs >> 30) & 1;
}

int BCS(){
	return (cprs >> 29) & 1;
}

int BCC(){
	return (~cprs >> 29) & 1;
}

int BMI(){
	return (cprs >> 31) & 1;
}

int BPL(){
	return (~cprs >> 31) & 1;
}

int BVS(){
	return (cprs >> 28) & 1;
}

int BVC(){
	return (~cprs >> 28) & 1;
}

int BHI(){
	return BCS() & BNE();
}

int BLS(){
	return BCC() & BEQ();
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
	return BEQ() & BLT();
}

void(*shifts[3])(int&, int, int) = { lsl, lsr, asr };
void(*arith[2])(int&, int, int) = { add, sub };
void(*movCompIpaddIpsub[4])(int&, int) = { mov, cmp, add8imm, sub8imm };
void(*logicalOps[16])(int&, int) = { TAND, TEOR, lslip, lsrip, asrip, adc, sbc, rorIP, tst, neg, cmpReg, cmnReg, ORR, mul, bic, mvn };
void(*hlOps[4])(int&, int) = { addNoCond, cmpHL, movNoCond, bx };
int(*conditions[16])() = { BEQ, BNE, BCS, BCC, BMI, BPL, BVS, BVC, BHI, BLS, BGE, BLT, BGT, BLE };

void moveShiftedRegister(int opcode){
	int rd = opcode & 0x7; //register, destination
	int rs = (opcode >> 3) & 0x7; //register, source
	int instruction = (opcode >> 11) & 3;
	int immediate = (opcode >> 6) & 0x1F;
	shifts[instruction](*r[rd], *r[rs], immediate);
}

void addSubFunction(int opcode){
	int rd = opcode & 0x7; //register, destination
	int rs = (opcode >> 3) & 0x7; //register, source
	int operation = (opcode >> 9) & 1;
	int immediateFlag = (opcode >> 10) & 1;
	int immediate = (opcode >> 6) & 7;
	int value = (immediateFlag == 1) ? immediate : *r[immediate];
	arith[operation](*r[rd], *r[rs], value);
}

void movCompSubAddImm(int opcode){
	int instruction = (opcode >> 11) & 3;
	int rd = (opcode >> 8) & 7;
	int immediate = (opcode & 0xFF);
	movCompIpaddIpsub[instruction](*r[rd], immediate);
}

void aluOps(int opcode){
	int instruction = (opcode >> 6) & 0xF;
	int rd = opcode & 0x07; //register, destination
	int rs = (opcode >> 3) & 7; //register, source
	logicalOps[instruction](*r[rd], *r[rs]);
}

void hiRegOperations(int opcode){
	int instruction = (opcode >> 8) & 3;
	int rd = opcode & 0x07; //register, destination
	int rs = (opcode >> 3) & 0xF; //register, source, exceptionally 4 bits as this opcode can access r0-r15
	int hi1 = ((opcode >> 4) & 8); //high reg flags enables access to r8-r15 registers
	hlOps[instruction](*r[rd | hi1], (rs == 15) ? ((*r[PC] + 2) & ~1) : *r[rs]);// PC as operand, broken?
}

void PCRelativeLoad(int opcode){
	//check this out later, memory masking?, potentially broken.
	int tmpPC = (*r[PC] + 2) & ~2;
	int rs = (opcode >> 8) & 7;
	int immediate = (opcode & 0xFF) << 2; //8 bit value to 10 bit value, last bits are 00 to be word alinged
	tmpPC += immediate;
	*r[rs] = loadFromAddress32(tmpPC);
}

void loadStoreRegOffset(int opcode){
	int rd = opcode & 7;
	int rb = (opcode >> 3) & 7; // base reg
	int ro = (opcode >> 6) & 7; // offset reg
	int byteFlag = (opcode >> 10) & 1;
	int loadFlag = (opcode >> 11) & 1;
	if (!loadFlag)
		byteFlag ? writeToAddress(*r[ro] + *r[rb], *r[rd]) : writeToAddress32(*r[ro] + *r[rb], *r[rd]);
	else
		*r[rd] = byteFlag ? loadFromAddress(*r[rb] + *r[ro]) : loadFromAddress32(*r[rb] + *r[ro]);
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
}

void loadSPRelative(int opcode){
	int loadFlag = (opcode >> 11) & 1;
	int rd = (opcode >> 8) & 0x07;
	int immediate = (opcode & 0xFF) << 2;
	if (loadFlag)
		*r[rd] = loadFromAddress32(*r[SP] + immediate);
	else
		writeToAddress32(*r[SP] + immediate, *r[rd]);
}

void loadAddress(int opcode){
	int rs = ((opcode >> 11) & 1) ? *r[SP] : ((*r[PC] & ~2) + 4);
	int rd = (opcode >> 8) & 0x07;
	int immediate = (opcode & 0xFF) << 2;
	*r[rd] = immediate + rs;
}

void addOffsetToSP(int opcode){
	int loadFlag = (opcode >> 7) & 1;
	int immediate = (opcode & 0x7F) << 2;
	*r[SP] += loadFlag ? -immediate : immediate;
}

void pushpop(int opcode){
	int immediate = opcode & 0xFF;
	int popFlag = (opcode >> 11) & 1;
	if (popFlag){
		for (int i = 0; i < 8; i++){
			if (immediate & 1){
				*r[i] = POP();
				//std::cout << "r" << i << "\n";
			}
			immediate = immediate >> 1;
		}
		*r[PC] = ((opcode >> 8) & 1) ? (POP() & -2) : (*r[PC]);
		//((opcode >> 8) & 1) ? std::cout << "PC\n" : std::cout << "";

	}
	else{
		if ((opcode >> 8) & 1)
			PUSH(*r[LR]);
		for (int i = 0; i < 8; i++){
			if (immediate & 0x80)
				PUSH(*r[7-i]);
			immediate = immediate << 1;
		}

	}
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
}

void conditionalBranch(int opcode){
	int immediate = opcode & 0xFF;
	int condition = (opcode >> 8) & 0x0F;
	*r[PC] += conditions[condition]() ? (((__int8)immediate << 1) + 2) : 0;
}

void unconditionalBranch(int opcode){
	int immediate = (opcode & 0x7FF) << 1;
	*r[PC] += signExtend<12>(immediate) + 2;
}

void branchLink(int opcode){
	int HLOffset = (opcode >> 11) & 1;
	int immediate = (opcode & 0x7FF);
	if (!HLOffset)
		*r[LR] = (signExtend<11>(immediate) << 12) + *r[PC] + 2;
	else{
		int nextInstruction = *r[PC] + 1;
		*r[PC] = *r[LR] + (immediate << 1); //maybe wrong check later
		*r[LR] = nextInstruction | 1;
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
						interruptController(opcode);					
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

