#include "iostream"
#include "GBAcpu.h"
#include "ThumbOpCodes.h"
#include "armopcodes.h"
#include "MemoryOps.h"
#include "Constants.h"
#include "interrupt.h"
#include "conditions.h"
#include "arithmeticOps.h"
#include "logicalOps.h"
#include <stdint.h>

void ARMBranch(int opCode){
    int location = opCode & 0xFFFFFF; //24 bits
	location = (location << 2) + 4;
    *r[PC] += signExtend<26>(location);
	if (debug)
		std::cout << conditions_s[opCode >> 28 & 0xF] << " " << r[PC] << " " << conditions[opCode >> 28 & 0xF]() << " ";
}

void incrementBase(int& baseRegister, bool nullParameter = false){
    baseRegister += 4;
}

void decrementBase(int& baseRegister, bool nullParameter = false){
    baseRegister -= 4;
}

template <typename function1, typename function2>
void BlockDataTransferSave(int opCode, function1 a, function2 b){
    __int16 baseReg = (opCode >> 16) & 15;
    int upDownBit = (opCode >> 23) & 1;
    int writeBack = (opCode >> 21) & 1;
	int usrMode = (opCode >> 22) & 1;
	__int16 regList = opCode & 0xFFFF;
    int oldBase = *r[baseReg];

	__int32** currentMode = r;

	if (usrMode && !((opCode >> 15) & 1)){
		r = usrSys;
	}

	if (debug)
		std::cout << "push ";

    if(((opCode >> 15)&1) & ~upDownBit){
        a(*r[baseReg], *r[15] + 8);
        b(*r[baseReg], *r[15] + 8);
    }

    for(int i = 0; i < 15; i++){
		if (upDownBit){
			if (regList & 1){
				if (i == 13 && baseReg == 13){
					a(*r[baseReg], oldBase);
					b(*r[baseReg], oldBase);
				}
				else{
					a(*r[baseReg], *r[i]);
					b(*r[baseReg], *r[i]);
				}
				if (debug)
					std::cout << "r" << i << " ";
			}
			regList >>= 1;
		}
		//we are pushing from higer registers first
		else if (~upDownBit){
			if (regList & 0x4000){
				if (i == 1 && baseReg == 13){
					a(*r[baseReg], oldBase);
					b(*r[baseReg], oldBase);
				}
				else{
					a(*r[baseReg], *r[14 - i]);
					b(*r[baseReg], *r[14 - i]);
				}
				if (debug)
					std::cout << "r" << 14 - i << " ";
			}
			regList <<= 1;
		}
    }

    if(((opCode >> 15)&1) & upDownBit){
        a(*r[baseReg], *r[15] + 8);
        b(*r[baseReg], *r[15] + 8);
    }

	if (usrMode){
		r = currentMode;
	}

	*r[baseReg] = writeBack ? *r[baseReg] : oldBase;
}

template <typename function1, typename function2>
void BlockDataTransferLoadPost(int opCode, function1 a, function2 b){ // not tested for r15
	int baseReg = (opCode >> 16) & 15;
	int upDownBit = (opCode >> 23) & 1;
	int writeBack = (opCode >> 21) & 1;
	int usrMode = (opCode >> 22) & 1;
	int regList = opCode & 0xFFFF;
	int oldBase = *r[baseReg];
	
	__int32** currentMode = r;

	if (usrMode && !((opCode >> 15) & 1)){
		r = usrSys;
	}

	if (debug)
		std::cout << "pop ";

	for (int i = 0; i < 16; i++){
		if (upDownBit){
			if (regList & 1){
				*r[i] = a(*r[baseReg], false);
				b(*r[baseReg], false);
				if (debug)
					std::cout << "r" << i << " ";
			}
			regList >>= 1;
		}
		else if (~upDownBit){
			if (regList & 0x8000){
				*r[15-i] = a(*r[baseReg], false);
				b(*r[baseReg], false);
				if (debug)
					std::cout << "r" << 14 - i << " ";
			}
			regList <<= 1;
		}
	}

	if (usrMode && !((opCode >> 15) & 1)){
		r = currentMode;
	}

	*r[baseReg] = writeBack ? *r[baseReg] : oldBase;
}

template <typename function1, typename function2>
void BlockDataTransferLoadPre(int opCode, function1 a, function2 b){
	int baseReg = (opCode >> 16) & 15;
	int upDownBit = (opCode >> 23) & 1;
	int writeBack = (opCode >> 21) & 1;
	bool usrMode = (opCode >> 20) & 1;
	int regList = opCode & 0xFFFF;
	int oldBase = *r[baseReg];

	__int32** currentMode = r;

	if (usrMode && !((opCode >> 15) & 1)){
		r = usrSys;
	}

	for (int i = 0; i < 16; i++){
		if (upDownBit){
			if (regList & 1){
				a(*r[baseReg], false);
				*r[i] = b(*r[baseReg], false);
				if (i == 15)
					*r[16] = cpsr.val;
			}
			regList >>= 1;
		}
		else if (~upDownBit){
			if (regList & 0x8000){
				a(*r[baseReg], false);
				*r[15-i] = b(*r[baseReg], false);
				if (i == 0)
					*r[16] = cpsr.val;
			}
			regList <<= 1;
		}
	}

	if (usrMode && !((opCode >> 15) & 1)){
		r = currentMode;
	}

	*r[baseReg] = writeBack ? *r[baseReg] : oldBase;
}


void singleDataSwap(int opCode){
	uint32_t rm = opCode & 0xF;
	uint32_t rd = (opCode >> 12) & 0xF;
	uint32_t rn = (opCode >> 16) & 0xF;
	bool byteFlag = (opCode >> 22) & 1;

	uint32_t tmp = byteFlag ? loadFromAddress(*r[rn]) : loadFromAddress32(*r[rn]);
	byteFlag ? writeToAddress(*r[rn], *r[rm]) : writeToAddress32(*r[rn], *r[rm]);
	*r[rd] = tmp;

    std::cout << "swp r" << rd << " r" << rm << " r[" << rn << "] ";
}

void branchAndExhange(int opCode){
    *r[15] = *r[opCode & 15];
    cpsr.thumb = (*r[15] & 1);
    *r[15] = (*r[15] & ~1);
	if (debug)
		std::cout << "bx " << *r[15] << " ";
}

void lslCond(int &saveTo, int from, int immidiate) {
	uint64_t tmp = (unsigned)from;
	saveTo = tmp << immidiate;

	if (immidiate > 32)
		cpsr.carry = 0;
	else if (immidiate > 0)
		cpsr.carry = ((unsigned)tmp >> (32 - immidiate) & 1);
	negative(saveTo);
	zero(saveTo);
}

void lsrCond(int &saveTo, int from, int immidiate) {
	uint64_t tmp = (unsigned)from;
	saveTo = tmp >> immidiate;

	if (immidiate > 0)
		cpsr.carry = (tmp >> (immidiate - 1) & 1);
	negative(saveTo);
	zero(saveTo);
}

void asrCond(int &saveTo, int from, int immidiate) {
	int64_t tmp = from;
	saveTo = tmp >> immidiate;

	if (immidiate != 0)
		cpsr.carry = (tmp >> (immidiate - 1) & 1);
	zero(saveTo);
	negative(saveTo);
}

void rorCond(int &saveTo,int from, int immidiate){
	if (immidiate > 32){
		rorCond(saveTo, from, immidiate - 32);
	}
	else{
		if (immidiate > 0)
			cpsr.carry = (from >> (immidiate - 1) & 1);
		saveTo = ((unsigned)from >> immidiate) | ((unsigned)from << (32 - immidiate));
		negative(saveTo);
		zero(saveTo);
	}
}

void lslNoCond(int &saveTo, int from, int immidiate) {
	uint64_t tmp = (unsigned)from;
	saveTo = tmp << immidiate;
}

void lsrNoCond(int &saveTo, int from, int immidiate) {
	uint64_t tmp = (unsigned)from;
	saveTo = tmp >> immidiate;
}

void asrNoCond(int &saveTo, int from, int immidiate) {
	int64_t tmp = from;
	saveTo = tmp >> immidiate;
}

void rorNoCond(int &saveTo, int from, int immidiate){
	if (immidiate > 32){
		rorNoCond(saveTo, from, immidiate - 32);
	}
	else{
		saveTo = ((unsigned)from >> immidiate) | ((unsigned)from << (32 - immidiate));
	}
}

uint32_t RORnoCond(uint32_t immediate, uint32_t by){
	if (by > 32){
		RORnoCond(immediate, by - 32);
	}
	return (immediate >> by) | (immediate << (32 - by));
}

void rrx(int& saveTo, uint32_t from){
	saveTo = (cpsr.carry << 31) | (from >> 1);
	cpsr.carry = from & 1;
	zero(saveTo);
	negative(saveTo);
}

void(*ARMshifts[4])(int&, int, int) = { lslCond, lsrCond, asrCond, rorCond };
void(*ARMshiftsNoCond[4])(int&, int, int) = { lslNoCond, lsrNoCond, asrNoCond, rorNoCond };
char* ARMshifts_s[4] = { "lsl", "lsr", "asr", "ror" };

void updateMode(){
	//std::cout << "switched mode to " << mode << std::endl;
	switch (cpsr.mode){
		case USR:
			r = usrSys;
			break;
		case FIQ:
			r = fiq;
			break;
		case IRQ:
			r = irq;
			break;
		case SUPER:
			r = svc;
			break;
		case ABORT:
			r = abt;
			break;
		case UNDEF:
			r = undef;
			break;
		case SYS:
			r = usrSys;
			break;
	}
}

void msr(int& saveTo, int operand1, int operand2){
	cpsr.val = operand2;
	updateMode();
}

void msr2(int& saveTo, int operand1, int operand2){
	*r[16] = operand2;
	updateMode();
}

void MSR(uint32_t opCode){
	bool SPSR = (opCode >> 22) & 1;
	union CPSR tmp_cpsr;
	uint8_t rotate = (opCode >> 8) & 0xF;
	uint32_t imm = opCode & 0xFF;
	uint32_t shiftedImm = RORnoCond(imm, rotate);
	shiftedImm = RORnoCond(shiftedImm, rotate);
	tmp_cpsr.val = shiftedImm;

	if (SPSR)
		*r[16] = shiftedImm;
	else {
		cpsr.zero = tmp_cpsr.zero;
		cpsr.overflow = tmp_cpsr.overflow;
		cpsr.carry = tmp_cpsr.carry;
		cpsr.negative = tmp_cpsr.negative;
	}

	if (debug)
		std::cout << "MSR " << (SPSR ? "SPSR " : "CPSR ") << std::hex << shiftedImm << std::dec << " ";
}


void mrs(int& saveTo, int operand1, int operand2){
	saveTo = cpsr.val;
}

void mrs2(int& saveTo, int operand1, int operand2){
	saveTo = *r[16];
}

void(*dataOperations[0x20])(int&, int, int) = {And, Ands, Eor, Eors, Sub, Subs, Rsb, Rsbs,
Add, Adds, Adc, Adcs, Sbc, Sbcs, Rsc, Rscs, mrs, Tst, msr, Teq, mrs2,
Cmp, msr2, Cmn, Orr, Orrs, Mov, Movs, Bic, Bics, Mvn, Mvns };

char* dataOperations_s[0x20] = { "and", "ands", "or", "ors", "sub", "subs", "rsb", "rsbs",
"add", "adds", "adc", "adcs", "sbc", "sbcs", "rsc", "rscs", "tst", "tst", "msr", "teq", "cmp",
"cmp", "msr", "cmn", "or", "ors", "mov", "movs", "bic", "bics", "mvn", "mvns" };

void immediateRotate(int opCode){
	bool codeExecuted = false;
	if (((opCode >> 12) & 0x3FF) == 0x28f && ((opCode >> 23) & 3) == 2 && ((opCode >> 26) & 3) == 0 && !codeExecuted)
	{
		int sprs = (opCode >> 22) & 1;
		int rm = opCode & 0xF;

		if (sprs){
			int tmp = cpsr.val & 0xFFFFFFF;
			tmp |= *r[rm] & 0xF0000000;
			*r[16] = tmp;
		}
		else{
			int tmp = cpsr.val & 0xFFFFFFF;
			tmp |= *r[rm] & 0xF0000000;
			cpsr.val = tmp;
		}
		codeExecuted = true;
	}

	if (!codeExecuted) {
		int rd = (opCode >> 12) & 15; //destination
		int rs = (opCode >> 16) & 15; //first operand
		int rn = opCode & 15; //2nd operand
		int tmpRegister = *r[rn];
		int immediate = (opCode >> 7) & 0x1F;
		int shiftId = (opCode >> 5) & 3;
		int operationID = (opCode >> 20) & 0x1F;
		int conditions = (opCode >> 20) & 1;
		int operand1 = *r[rs];

		if (rn == 15)
			tmpRegister += 4;
		else if (rs == 15)
			operand1 += 4;

		if (shiftId == 3 && immediate == 0){
			rrx(tmpRegister, tmpRegister);
		}
		else{
			if (immediate == 0 && shiftId != 0)
				immediate = 0x20;
			if (conditions && (((operationID > 3) && (operationID < 16)) || ((operationID > 21) && (operationID < 24))))
				ARMshiftsNoCond[shiftId](tmpRegister, tmpRegister, immediate);
			else if (conditions)
				ARMshifts[shiftId](tmpRegister, tmpRegister, immediate);
			else
				ARMshiftsNoCond[shiftId](tmpRegister, tmpRegister, immediate);
		}

		dataOperations[operationID](*r[rd], operand1, tmpRegister);

		if (rd == 15 && (opCode >> 20) & 1){
			cpsr.val = *r[16];
			updateMode();
		}
		
		if (debug)
			std::cout << dataOperations_s[operationID] << " r" << rd << ", r" << rs << ", r" << rn << ", " << ARMshifts_s[shiftId] << " =" << immediate << " ";
		
	}
}

void registerRotate(int opCode){
	int rd = (opCode >> 12) & 0xF; //destination
	int rn = (opCode >> 16) & 0xF; //first operand
	int operand = opCode & 0xFF; //2nd operand bytes
	int rm = opCode & 0xF;
	int rs = (opCode >> 8) & 0xF;
	int shiftId = (opCode >> 5) & 3;
	int shiftAmount = *r[rs] & 0xFF;
	int tmpResult = 0;
	int operationID = (opCode >> 20) & 0x1F;
	int conditions = (opCode >> 20) & 1;

	if (rm == 15) // not tested
		*r[rm] += 8;
	else if (rn == 15)
		*r[rn] += 8;

	if (conditions && (((operationID > 3) && (operationID < 16)) || ((operationID > 21) && (operationID < 24))))
		ARMshiftsNoCond[shiftId](tmpResult, *r[rm], shiftAmount);
	else if (conditions)
		ARMshifts[shiftId](tmpResult, *r[rm], shiftAmount);
	else
		ARMshiftsNoCond[shiftId](tmpResult, *r[rm], shiftAmount);

	dataOperations[operationID](*r[rd], *r[rn], tmpResult);

	if (rd == 15 && (opCode >> 20) & 1){ // not tested
		cpsr.val = *r[16];
		updateMode();
	}

	if (debug)
		std::cout << dataOperations_s[operationID] << " r" << rd << ", r" << rn << ", r" << rm << " " << ARMshifts_s[shiftId] << " " << operand << " ";

}

void dataProcessingImmediate(int opCode){
	int rd = (opCode >> 12) & 0xF; //destination
	int rs = (opCode >> 16) & 0xF; //first operand
	int operand1 = *r[rs];
	int immediate = opCode & 0xFF;
	if (rs == 15)
		operand1 += 4;
	int shift = (opCode >> 8) & 0xF;
	int shiftedImm = 0;
	bool conditions = (opCode >> 20) & 1;
	int operationID = (opCode >> 20) & 0x1F;

	if (conditions && (((operationID > 3) && (operationID < 16)) || ((operationID > 21) && (operationID < 24)))){
		shiftedImm = RORnoCond(immediate, shift);
		shiftedImm = RORnoCond(shiftedImm, shift);
	}
	else if (conditions){
		rorCond(shiftedImm, immediate, shift);
		rorCond(shiftedImm, shiftedImm, shift);
	}
	else{
		rorNoCond(shiftedImm, immediate, shift);
		rorNoCond(shiftedImm, shiftedImm, shift);
	}
	
	dataOperations[operationID](*r[rd], operand1, shiftedImm);

	if (rd == 15 && conditions){
		cpsr.val = *r[16];
		updateMode();
	}

	if (debug)
		std::cout << dataOperations_s[operationID] << " r" << rd << ", r" << rs << ", " << shiftedImm << " ";
}

void halfDataTransfer(int opCode){
	int pFlag = (opCode >> 24) & 1;
	int uFlag = (opCode >> 23) & 1;
	int func = (opCode >> 22) & 1;
	int wFlag = (opCode >> 21) & 1;
	int lFlag = (opCode >> 20) & 1;
	int shFlag = (opCode >> 5) & 3;
	int rn = (opCode >> 16) & 0xF;
	int rd = (opCode >> 12) & 0xF;
	int offset = (opCode >> 4) & 0xF0 | opCode & 0xF;
	offset += (rn == 15) ? 8 : 0; //8 or 4? 
	int calculated = (rd == 15) ? (*r[rn] + 8) : *r[rn];

	switch (func){
		case 0:
			if (lFlag){
				if(pFlag) 
					calculated += uFlag ? *r[offset] : -*r[offset];
				if (shFlag == 1)
					*r[rd] = loadFromAddress16(calculated);
				else if (shFlag == 2)
					*r[rd] = signExtend<8>(loadFromAddress(calculated));
				else
					if (calculated & 1){
						*r[rd] = loadFromAddress16(calculated);
						if (*r[rd] & 0x80) //sign bit on
							*r[rd] |= 0xFFFFFF00;
					}
					else
						*r[rd] = signExtend<16>(loadFromAddress16(calculated));
				if (!pFlag)
					calculated += uFlag ? *r[offset] : -*r[offset];
			}
			else{
				if (pFlag)
					calculated += uFlag ? *r[offset] : -*r[offset];
				if (shFlag == 1)
					writeToAddress16(calculated, *r[rd]);
				else if (shFlag == 2)
					writeToAddress(calculated, signExtend<8>(*r[rd]));
				else
					writeToAddress16(calculated, signExtend<16>(*r[rd]));
				if (!pFlag)
					calculated += uFlag ? *r[offset] : -*r[offset];
			}
			*r[rn] = (wFlag || !pFlag) ? calculated : *r[rn];
			break;
		case 1:
			if (lFlag){
				if (pFlag)
					calculated += uFlag ? offset : -offset;
				if (shFlag == 1)
					*r[rd] = loadFromAddress16(calculated);
				else if (shFlag == 2)
					*r[rd] = signExtend<8>(loadFromAddress(calculated));
				else
					if (calculated & 1){
						*r[rd] = loadFromAddress16(calculated);
						if (*r[rd] & 0x80) //sign bit on
							*r[rd] |= 0xFFFFFF00;
					}
					else
						*r[rd] = signExtend<16>(loadFromAddress16(calculated));
				if (!pFlag)
					calculated += uFlag ? offset : offset;
			}
			else{
				if (pFlag)
					calculated += uFlag ? offset : -offset;
				if (shFlag == 1)
					writeToAddress16(calculated, *r[rd]);
				else if (shFlag == 2)
					writeToAddress(calculated, signExtend<8>(*r[rd]));
				else
					writeToAddress16(calculated, signExtend<16>(*r[rd]));
				if (!pFlag)
					calculated += uFlag ? offset : -offset;
			}
			*r[rn] = (wFlag || !pFlag) ? calculated : *r[rn];
			break;
	}
	
}

void multiply(int opCode){
	int accumulateBit = (opCode >> 21) & 1;
	int setBit = (opCode >> 20) & 1;
	int rd = (opCode >> 16) & 0xF;
	int rn = (opCode >> 12) & 0xF;
	int rs = (opCode >> 8) & 0xF;
	int rm = opCode & 0xF;
	*r[rd] = accumulateBit ? *r[rm] * *r[rs] + *r[rn] : *r[rm] * *r[rs];
	if (setBit){
		negative(*r[rd]);
		zero(*r[rd]);
	}
	if (debug)
		std::cout << "mult ";
}

void multiplyLong(int opCode){
	int sign = (opCode >> 22 ) & 1;
	int accumulate = (opCode >> 21) & 1;
	int setConditions = (opCode >> 20) & 1;

	uint32_t rdHi = (opCode >> 16) & 0xF;
	uint32_t rdLo = (opCode >> 12) & 0xF;
	uint32_t Rs = (opCode >> 8) & 0xF;
	uint32_t Rm = opCode & 0xF;
	uint64_t tmp = 0;
	uint64_t tmp4 = (((uint64_t)*r[rdHi] & 0xFFFFFFFF) << 32) | (*r[rdLo] & 0xFFFFFFFF);
	if (sign){
		int64_t tmp2 = *r[Rs];
		int64_t tmp3 = *r[Rm];
		tmp = tmp2 * tmp3;
		if (accumulate)
			tmp = tmp + tmp4;
	}
	else{
		uint64_t tmp2 = *r[Rs] & 0xFFFFFFFF;
		uint64_t tmp3 = *r[Rm] & 0xFFFFFFFF;
		tmp = (uint64_t)tmp2 * (uint64_t)tmp3;
		if (accumulate)
			tmp = (uint64_t)tmp + (uint64_t)tmp4;
	}

	*r[rdHi] = (tmp >> 32) & 0xFFFFFFFF;
	*r[rdLo] = tmp & 0xFFFFFFFF;

	if (setConditions){
		negative(*r[rdHi]);
		zero(tmp);
	}
	if (debug)
		std::cout << "mult long ";
}

void singleDataTrasnferImmediatePre(int opCode){
	int calculated = 0;
	int upDownBit = (opCode >> 23) & 1;
	int byteFlag = (opCode >> 22) & 1;
	int writeBack = (opCode >> 21) & 1;
	int loadStore = (opCode >> 20) & 1;
	int baseReg = (opCode >> 16) & 15;
	int destinationReg = (opCode >> 12) & 15;
	int offset = opCode & 0xFFF;
	offset += (baseReg == 15) ? 4 : 0;
	int oldReg = *r[baseReg];
	switch (loadStore){
	case 0:
		*r[baseReg] += upDownBit ? offset : -offset;
		calculated = *r[baseReg];
		*r[baseReg] = (writeBack)? *r[baseReg] : oldReg;
		if (destinationReg == 15)
			*r[destinationReg] += 8;
		byteFlag ? writeToAddress(calculated, *r[destinationReg]) : writeToAddress32(calculated, *r[destinationReg]);
		if (destinationReg == 15)
			*r[destinationReg] -= 8;
		break;
	case 1:
		*r[baseReg] += upDownBit ? offset : -offset;
		calculated = *r[baseReg];
		*r[baseReg] = (writeBack) ? *r[baseReg] : oldReg;
		*r[destinationReg] = byteFlag ? loadFromAddress(calculated) : loadFromAddress32(calculated);
		break;
	}

	cycles += S_cycles + N_cycles + 1;
	
	if (debug && loadStore)
		std::cout << "ldr r" << destinationReg << " [r" << baseReg << " =" << (upDownBit ? offset : -offset) << "] ";
	else if (debug && !loadStore)
		std::cout << "str [r" << baseReg << " " << (upDownBit ? offset : -offset) << "] r" << destinationReg << " ";
}

void singleDataTrasnferImmediatePost(int opCode){
	int upDownBit = (opCode >> 23) & 1;
	int byteFlag = (opCode >> 22) & 1;
	int loadStore = (opCode >> 20) & 1;
	int baseReg = (opCode >> 16) & 15;
	int destinationReg = (opCode >> 12) & 15;
	int offset = opCode & 0xFFF;
	offset += (baseReg == 15) ? 4 : 0; //for PC as offset, remember that PC is behind
	int calculated = *r[baseReg];
	switch (loadStore){
	case 0:
		if (destinationReg == 15)
			*r[destinationReg] += 8;
		byteFlag ? writeToAddress(calculated, *r[destinationReg]) : writeToAddress32(calculated, *r[destinationReg]);
		calculated += upDownBit ? offset : -offset;
		if (destinationReg == 15)
			*r[destinationReg] -= 8;
		break;
	case 1:
		*r[destinationReg] = byteFlag ? loadFromAddress(calculated) : loadFromAddress32(calculated);
		calculated += upDownBit ? offset : -offset;
		break;
	}
	*r[baseReg] = calculated;

	cycles += S_cycles + N_cycles + 1;
}

void singleDataTrasnferRegisterPre(int opCode){
	int offset = 0;

	int rn = (opCode >> 16) & 0xF;
	int rd = (opCode >> 12) & 0xF;
	int rm = opCode & 0xF;
	int rs = (opCode >> 8) & 0xF;

	int upDownBit = (opCode >> 23) & 1;
	int byteFlag = (opCode >> 22) & 1;
	int writeBack = (opCode >> 21) & 1;
	int loadStore = (opCode >> 20) & 1;

	int shiftId = (opCode >> 5) & 3;

	if ((opCode >> 4) & 1){
		std::cout << "TBD";
	}

	else
	{
		offset = (opCode >> 7) & 0x1F;
		ARMshifts[shiftId](offset, *r[rm], offset);
	}

	int oldReg = *r[rn];
	switch (loadStore){
		case 0:
			if (rd == 15)
				*r[rd] += 8;
			*r[rn] += upDownBit ? offset : -offset;
			byteFlag ? writeToAddress(*r[rn], *r[rd]) : writeToAddress32(*r[rn], *r[rd]);
			*r[rn] = (writeBack) ? *r[rn] : oldReg;
			break;

		case 1:
			*r[rn] += upDownBit ? offset : -offset;
			*r[rd] = byteFlag ? loadFromAddress(*r[rn]) : loadFromAddress32(*r[rn]);
			*r[rn] = (writeBack) ? *r[rn] : oldReg;
			if (debug && byteFlag)
				std::cout << "ldrb r" << rd << ", [r" << rn << " r" << rm << "] ";
			else if (debug && !byteFlag)
				std::cout << "ldr r" << rd << ", [r" << rn << " r" << rm << "] ";
			break;
	}

	cycles += S_cycles + N_cycles + 1;
}

void singleDataTrasnferRegisterPost(int opCode){
	int upDownBit = (opCode >> 23) & 1;
	int byteFlag = (opCode >> 22) & 1;
	int writeback = (opCode >> 21) & 1;
	int loadStore = (opCode >> 20) & 1;
	int baseReg = (opCode >> 16) & 15;
	int destinationReg = (opCode >> 12) & 15;
	int shiftAmount = (opCode >> 4) & 0xFF;
	int offset = *r[(opCode & 0xF)] << shiftAmount;
	offset += (baseReg == 15) ? 4 : 0; //for PC as offset, remember that PC is behind

	int oldReg = *r[baseReg];

	switch (loadStore){
	case 0:
		if (destinationReg == 15)
			*r[destinationReg] += 8;
		byteFlag ? writeToAddress(*r[baseReg], *r[destinationReg]) : writeToAddress32(*r[baseReg], *r[destinationReg]);
		*r[baseReg] += upDownBit ? offset : -offset;
		if (destinationReg == 15)
			*r[destinationReg] -= 8;
		break;
	case 1:
		*r[destinationReg] = byteFlag ? loadFromAddress(*r[baseReg]) : loadFromAddress32(*r[baseReg]);
		*r[baseReg] += upDownBit ? offset : -offset;
		break;
	}

	cycles += S_cycles + N_cycles + 1;
}

void ARMExecute(int opCode){
    int condition = (opCode >> 28) & 0xF;
	*r[PC] += 4;
	cycles += 1;
	if (conditions[condition]()) //condition true
    {
        int opCodeType = (opCode >> 24) & 0xF;
        int subType;
        switch(opCodeType){
			case 15:
				interruptController();
				break;
			case 14: //coProcessor data ops / register transfer, not used in GBA
				break;
			case 13: case 12: //co processor data transfer, not used in GBA
				break;
			case 11: //branch with link 
				*r[LR] = *r[PC];
				*r[LR] &= ~3; //bits 1-0 should always be cleared, but you never know
			case 10://branch 
				ARMBranch(opCode);
				break;
			case 9: //block data transfer pre offset. maybe implement S bits
				subType = (opCode >> 20) & 0xB;
				switch(subType){
					case 10: case 8: //writeback / no writeback, pre offset, add offset
						BlockDataTransferSave(opCode, incrementBase, writeToAddress32);
						break;
					case 2: case 0: //writeback / no writeback, pre offset, sub offset
						BlockDataTransferSave(opCode, decrementBase, writeToAddress32);
						break;
					case 11: case 9: //writeback / no writeback, post offset, add offset
						BlockDataTransferLoadPre(opCode, incrementBase, loadFromAddress32);
						break;
					case 3: case 1: //writeback / no writeback, post offset, sub offset
						BlockDataTransferLoadPre(opCode, decrementBase, loadFromAddress32);
						break;
				}
				break;
			case 8: //block data transfer post offset
				subType = (opCode >> 20) & 0xB;
				switch(subType){
					case 10: case 8: //writeback / no writeback, post offset, add offset
						BlockDataTransferSave(opCode, writeToAddress32, incrementBase);
						break;
					case 2: case 0: //writeback / no writeback, post offset, sub offset
						BlockDataTransferSave(opCode, writeToAddress32 , decrementBase);
						break;
					case 11: case 9: //writeback / no writeback, post offset, add offset
						BlockDataTransferLoadPost(opCode, loadFromAddress32, incrementBase);
						break;
					case 3: case 1: //writeback / no writeback, post offset, sub offset
						BlockDataTransferLoadPost(opCode, loadFromAddress32, decrementBase);
						break;
				}
				break;
			case 7:// single data transfer, register pre offset 
				singleDataTrasnferRegisterPre(opCode);
				break;
			case 6:// single data transfer, register, post offset
				singleDataTrasnferRegisterPost(opCode);
				break;
			case 5:// single data transfer, immediate pre offset
				singleDataTrasnferImmediatePre(opCode);
				break;
			case 4: // single data transfer, immediate post offset
				singleDataTrasnferImmediatePost(opCode);
				break;
			case 3: case 2: //data processing, immediate check msr?
				if ((((opCode >> 12) & 0x3FF) == 0x28F) && (((opCode >> 23) & 0x3) == 2) && (((opCode >> 26) & 0x3) == 0))
					MSR(opCode);
				else
					dataProcessingImmediate(opCode);
				break;
			case 1: case 0: //data prceossing, multiply, data transfer, branch and exhange
				if(((opCode >> 4) & 0x12FFF1) == 0x12FFF1)
					branchAndExhange(opCode);
				else if(((opCode >> 4) & 1) == 0) //data processing
					immediateRotate(opCode);
				else if(((opCode >> 7) & 1) == 0) //data processing
					registerRotate(opCode);
				else if ((((opCode >> 23) & 0x1F) == 2) && (((opCode >> 4) & 0xFF) == 9))
					singleDataSwap(opCode);
				else if (((opCode >> 23) & 0x1F) == 0 && (((opCode >> 4) & 0xF) == 9))
					multiply(opCode);
				else if (((opCode >> 23) & 0x1F) == 1 && (((opCode >> 4) & 0xF) == 9))
					multiplyLong(opCode);
				else
				    halfDataTransfer(opCode);
				break;
		}
	}
}
