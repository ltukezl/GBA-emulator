#include "iostream"
#include "GBAcpu.h"
#include "ThumbOpCodes.h"
#include "MemoryOps.h"
#include "Constants.h"
#include "interrupt.h"

int noCond(){
    return 1;
}

int(*ARMconditions[16])() = { BEQ, BNE, BCS, BCC, BMI, BPL, BVS, BVC, BHI, BLS, BGE, BLT, BGT, BLE, noCond };

void ARMBranch(int opCode){
    int location = opCode & 0xFFFFFF; //24 bits
	location = (location << 2) + 4;
    *r[PC] += signExtend<26>(location);
	if (debug)
		std::cout << "bx " << r[PC];
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
	__int16 regList = opCode & 0xFFFF;
    int oldBase = *r[baseReg];

    if(((opCode >> 15)&1) & ~upDownBit){
        a(*r[baseReg], *r[15] + 12);
        b(*r[baseReg], *r[15] + 12);
    }

    for(int i = 0; i <15; i++){
		if (upDownBit){
			if (regList & 1){
				a(*r[baseReg], *r[i]);
				b(*r[baseReg], *r[i]);
			}
			regList >>= 1;
		}
		//we are pushing from higer registers first
		else if (~upDownBit){
			if (regList & 0x4000){
				a(*r[baseReg], *r[14-i]);
				b(*r[baseReg], *r[14-i]);
			}
			regList <<= 1;
		}
    }

    if(((opCode >> 15)&1) & upDownBit){
        a(*r[baseReg], *r[15] + 12);
        b(*r[baseReg], *r[15] + 12);
    }

    *r[baseReg] = writeBack ? *r[baseReg] : oldBase;
}

template <typename function1, typename function2>
void BlockDataTransferLoadPost(int opCode, function1 a, function2 b){ // not tested for r15
	int baseReg = (opCode >> 16) & 15;
	int upDownBit = (opCode >> 23) & 1;
	int writeBack = (opCode >> 21) & 1;
	int regList = opCode & 0xFFFF;
	int oldBase = *r[baseReg];
	
	for (int i = 0; i < 15; i++){
		if (upDownBit){
			if (regList & 1){
				*r[i] = a(*r[baseReg], false);
				b(*r[baseReg], false);
			}
			regList >>= 1;
		}
		else if (~upDownBit){
			if (regList & 0x4000){
				*r[15-i] = a(*r[baseReg], false);
				b(*r[baseReg], false);
			}
			regList <<= 1;
		}
	}

	*r[baseReg] = writeBack ? *r[baseReg] : oldBase;
}

template <typename function1, typename function2>
void BlockDataTransferLoadPre(int opCode, function1 a, function2 b){ // not tested
	int baseReg = (opCode >> 16) & 15;
	int upDownBit = (opCode >> 23) & 1;
	int writeBack = (opCode >> 21) & 1;
	int regList = opCode & 0xFFFF;
	int oldBase = *r[baseReg];

	for (int i = 0; i < 15; i++){
		if (upDownBit){
			if (regList & 1){
				a(*r[baseReg], false);
				*r[i] = b(*r[baseReg], false);
			}
			regList >>= 1;
		}
		else if (~upDownBit){
			if (regList & 0x4000){
				a(*r[baseReg], false);
				*r[15-i] = b(*r[baseReg], false);
			}
			regList <<= 1;
		}
	}

	*r[baseReg] = writeBack ? *r[baseReg] : oldBase;
}


void singleDataSwap(int opCode){
    std::cout << "singleDataSwap\n";
	std::cin >> *r[0];
}

void branchAndExhange(int opCode){
    *r[15] = *r[opCode & 15];
    cpsr.thumb = (*r[15] & 1);
    *r[15] = (*r[15] & ~1);
	if (debug)
		std::cout << "bx " << *r[15] << " ";
}

void lslCond(int &saveTo, int from, int immidiate) {
	if (immidiate > 0)
		cpsr.carry = (from >> (32 - immidiate) & 1);
    saveTo = from << immidiate;
	zero(saveTo);
	negative(saveTo);
}

void lsrCond(int &saveTo, int from, int immidiate) {
	cpsr.carry = ((unsigned)from >> (immidiate - 1) & 1);
    saveTo = (unsigned)from >> immidiate;
	zero(saveTo);
	negative(saveTo);
}

void asrCond(int &saveTo, int from, int immidiate) {
	cpsr.carry = (from >> ((int)immidiate + 1) & 1);
    saveTo = from >> immidiate;
	zero(saveTo);
	negative(saveTo);
}

void rorCond(int &saveTo,int from, int immidiate){
	cpsr.carry = (from >> (immidiate - 1) & 1);
    saveTo = (from << immidiate) | (from >> (32 - immidiate));
	zero(saveTo);
	negative(saveTo);
}

void lslNoCond(int &saveTo, int from, int immidiate) {
	if (immidiate > 0)
		cpsr.carry = (from >> (32 - immidiate) & 1);
	saveTo = from << immidiate;
}

void lsrNoCond(int &saveTo, int from, int immidiate) {
	cpsr.carry = ((unsigned)from >> (immidiate - 1) & 1);
	saveTo = (unsigned)from >> immidiate;
}

void asrNoCond(int &saveTo, int from, int immidiate) {
	cpsr.carry = (from >> ((int)immidiate + 1) & 1);
	saveTo = from >> immidiate;
}

void rorNoCond(int &saveTo, int from, int immidiate){
	cpsr.carry = (from >> (immidiate - 1) & 1);
	saveTo = (from << immidiate) | (from >> (32 - immidiate));
}

void(*ARMshifts[4])(int&, int, int) = { lslCond, lsrCond, asrCond, rorCond };
void(*ARMshiftsNoCond[4])(int&, int, int) = { lslNoCond, lsrNoCond, asrNoCond, rorNoCond };
char* ARMshifts_s[4] = { "lsl", "lsr", "asr", "ror" };

void ARMAnd(int& saveTo, int operand1, int operand2){
    saveTo = operand1 & operand2;
}

void ARMAnds(int& saveTo, int operand1, int operand2){
    saveTo = operand1 & operand2;
    zero(saveTo);
    negative(saveTo);
}

void ARMEOR(int& saveTo, int operand1, int operand2){
    saveTo = operand1 ^ operand2;
}

void ARMEORS(int& saveTo, int operand1, int operand2){
    saveTo = operand1 ^ operand2;
    zero(saveTo);
    negative(saveTo);
}

void ARMSub(int& saveTo, int operand1, int operand2){
    saveTo = operand1 - operand2;
}

void ARMSubs(int& saveTo, int operand1, int operand2){
    saveTo = operand1 - operand2;
    zero(saveTo);
    negative(saveTo);
    subCarry(operand1, operand2, saveTo);
    subOverflow(operand1, operand2, saveTo);
}

void ARMRsb(int& saveTo, int operand1, int operand2){
    saveTo = operand2 - operand1;
}

void ARMRsbs(int& saveTo, int operand1, int operand2){
    saveTo = operand2 - operand1;
    zero(saveTo);
    negative(saveTo);
    subCarry(operand2, operand1, saveTo);
    subOverflow(operand2, operand1, saveTo);
}

void ARMAdd(int& saveTo, int operand1, int operand2){
    saveTo = operand1 + operand2;
}

void ARMAdds(int& saveTo, int operand1, int operand2){
    saveTo = operand1 + operand2;
    zero(saveTo);
    negative(saveTo);
    addCarry(operand2, operand1, saveTo);
    addOverflow(operand2, operand1, saveTo);
}

void ARMAdc(int& saveTo, int operand1, int operand2){
	saveTo = operand1 + operand2 + cpsr.carry;
}

void ARMAdcs(int& saveTo, int operand1, int operand2){
    int tmpOperand = saveTo;
	saveTo = operand1 + operand2 + cpsr.carry;
    zero(saveTo);
    negative(saveTo);
    addCarry(tmpOperand, operand1, saveTo);
    addOverflow(tmpOperand, operand1, saveTo);
}

void ARMSbc(int& saveTo, int operand1, int operand2){
    saveTo = operand1 - operand2 + cpsr.carry - 1;
}

void ARMSbcs(int& saveTo, int operand1, int operand2){
    int tmpOperand = saveTo;
	saveTo = operand1 - operand2 + cpsr.carry - 1;
    zero(saveTo);
    negative(saveTo);
    subCarry(tmpOperand, operand1, saveTo);
    subOverflow(tmpOperand, operand1, saveTo);
}

void ARMRsc(int& saveTo, int operand1, int operand2){
	saveTo = operand2 - operand1 + cpsr.carry - 1;
}

void ARMRscs(int& saveTo, int operand1, int operand2){
    int tmpOperand = saveTo;
    saveTo = operand2 - operand1 + cpsr.carry - 1;
    zero(saveTo);
    negative(saveTo);
    subCarry(operand2, tmpOperand, saveTo);
    subOverflow(operand2, tmpOperand, saveTo);
}

void ARMTST(int& saveTo, int operand1, int operand2){
    zero(operand1 & operand2);
    negative(operand1 & operand2);
}

void ARMTEQ(int& saveTo, int operand1, int operand2){
    zero(operand1 ^ operand2);
    negative(operand1 ^ operand2);
}

void ARMCMP(int& saveTo, int operand1, int operand2){
    zero(operand1 - operand2);
    negative(operand1 - operand2);
	subCarry(operand1, operand2, operand1 - operand2);
	subOverflow(operand1, operand2, operand1 - operand2);
}

void ARMCMN(int& saveTo, int operand1, int operand2){
    zero(operand1 + operand2);
    negative(operand1 + operand2);
	addCarry(operand1, operand2, operand1 + operand2);
	addOverflow(operand1, operand2, operand1 + operand2);
}

void ARMORR(int& saveTo, int operand1, int operand2){
    saveTo = operand1 | operand2;
}

void ARMORRS(int& saveTo, int operand1, int operand2){
    saveTo = operand1 | operand2;
    zero(saveTo);
    negative(saveTo);
}

void ARMMov(int& saveTo, int operand1, int operand2){
    saveTo = operand2;
}
void ARMMovs(int& saveTo, int operand1, int operand2){
    saveTo = operand2;
    zero(saveTo);
    negative(saveTo);
}

void ARMBic(int& saveTo, int operand1, int operand2){
    saveTo = operand1 & ~operand2;
}

void ARMBics(int& saveTo, int operand1, int operand2){
    saveTo = operand1 & ~operand2;
    zero(saveTo);
    negative(saveTo);
}

void ARMMvn(int& saveTo, int operand1, int operand2){
    saveTo = ~operand2;
}

void ARMMvns(int& saveTo, int operand1, int operand2){
    saveTo = ~operand2;
    zero(saveTo);
    negative(saveTo);
}

void updateMode(){
	//std::cout << "switched mode to " << mode << std::endl;
	switch (cpsr.mode){
		case USR:
			r = usrSys;
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

int ROR(unsigned int immediate, unsigned int by){
	if (by == 0)
		cpsr.carry = immediate >> 31 & 1;
	else
		cpsr.carry = (immediate >> (by - 1)) & 1;
	return (immediate >> by) | (immediate << (32 - by));
}

void(*dataOperations[0x20])(int&, int, int) = {ARMAnd, ARMAnds, ARMEOR, ARMEORS, ARMSub, ARMSubs, ARMRsb, ARMRsbs,
ARMAdd, ARMAdds, ARMAdc, ARMAdcs, ARMSbc, ARMSbcs, ARMRsc, ARMRscs, ARMTST, ARMTST, ARMTEQ, ARMTEQ, ARMCMP,
        ARMCMP, ARMCMN, ARMCMN, ARMORR, ARMORRS, ARMMov, ARMMovs, ARMBic, ARMBics, ARMMvn, ARMMvns};

char* dataOperations_s[0x20] = { "and", "ands", "or", "ors", "sub", "subs", "rsb", "rsbs",
"add", "adds", "adc", "adcs", "sbc", "sbcs", "rsc", "rscs", "tst", "tst", "teq", "teq", "cmp",
"cmp", "cmn", "cmn", "or", "ors", "mov", "movs", "bic", "bics", "mvn", "mvns" };

void immediateRotate(int opCode){
	bool codeExecuted = false;
	if (((opCode >> 23) & 0x1F) == 2){
		if ((opCode & 0xFFF) == 0 && (((opCode >> 16) & 0x3F) == 0xF)){
			int sprs = (opCode >> 22) & 1;
			int rm = (opCode >> 12) & 0xF;
			if (sprs)
				*r[rm] = *r[16];
			else
				*r[rm] = cpsr.val;
			codeExecuted = true;

			if (debug & sprs)
				std::cout << "mov r" << rm << ", spsr ";
			else if (debug & !sprs)
				std::cout << "mov r" << rm << ", cpsr_" << cpsr.mode << " ";
		}

		else if (((opCode >> 4) & 0x3FFFF) == 0x29f00){
			int sprs = (opCode >> 22) & 1;
			int rm = opCode & 0xF;
			if (sprs)
				*r[16] = *r[rm];
			else{
				cpsr.val = *r[rm];
				updateMode();
			}
			codeExecuted = true;

			if (debug & sprs)
				std::cout << "mov cpsr_" << cpsr.mode << ", spsr ";
			else if (debug & !sprs)
				std::cout << "mov cpsr_" << cpsr.mode << ", r" << rm << " ";
		}
	}

	if (((opCode >> 12) & 0x3FF) == 0x28f && ((opCode >> 23) & 3) == 2 && ((opCode >> 26) & 3) == 0 && !codeExecuted)
	{
		int immediate = (opCode >> 25) & 1;
		int sprs = (opCode >> 22) & 1;
		int rm = opCode & 0xF;

		if (immediate == 0){
			if (sprs){
				int tmp = cpsr.val & 0xFFFFFFF;
				tmp |= *r[rm] & 0xF0000000;
				*r[16] = tmp;
			}
			else{
				int tmp = cpsr.val & 0xFFFFFFF;
				tmp |= *r[rm] & 0xF0000000;
				cpsr.val = tmp;
				updateMode();
			}
		}
		else{
			std::cout << "TBD3";
		}
		codeExecuted = true;
	}

	if (!codeExecuted) {
		int rd = (opCode >> 12) & 15; //destination
		int rs = (opCode >> 16) & 15; //first operand
		int rn = opCode & 15; //2nd operand
		int tmpRegister = *r[rn];

		if (rn == 15 || rs == 15)
			tmpRegister += 4;

		int immediate = (opCode >> 7) & 0x1F;
		int shiftId = (opCode >> 5) & 3;
		int operationID = (opCode >> 20) & 0x1F;

		ARMshiftsNoCond[shiftId](tmpRegister, tmpRegister, immediate);
		dataOperations[operationID](*r[rd], *r[rs], tmpRegister);

		if (rd == 15 && (opCode >> 20) & 1){
			cpsr.val = cpsr.val;
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
	int shiftAmount = *r[rs] & 0xF;
	int operationID = (opCode >> 20) & 0x1F;

	if (rn == 15 || rs == 15) // not tested
		shiftAmount += 4;

	ARMshifts[shiftId](shiftAmount, *r[rm], shiftAmount);
	dataOperations[operationID](*r[rd], *r[rn], shiftAmount);

	if (rd == 15 && (opCode >> 20) & 1){ // not tested
		cpsr.val = cpsr.val;
		updateMode();
	}

	if (debug)
		std::cout << dataOperations_s[operationID] << " r" << rd << ", r" << rn << ", r" << rm << " " << ARMshifts_s[shiftId] << " " << operand << " ";

}

void dataProcessingImmediate(int opCode){
    int rd = (opCode >> 12) & 0xF; //destination
    int rs = (opCode >> 16) & 0xF; //first operand
	int operand1 = (rs == 15) ? *r[rs] + 4 : *r[rs];
    int immediate = opCode  & 0xFF;
    int shift = (opCode >> 8) & 0xF;
	int shiftedImm = ROR(immediate, shift);
	shiftedImm = ROR(shiftedImm, shift);
    int operationID = (opCode >> 20) & 0x1F;
    dataOperations[operationID](*r[rd], operand1, shiftedImm);
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
	offset += (rn == 15) ? 8 : 0;
	int calculated = (rd == 15) ? (*r[rn] + 12) : *r[rn];

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
			*r[rn] = wFlag ? calculated : *r[rn];
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
			*r[rn] = wFlag ? calculated : *r[rn];
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
}

void multiplyLong(int opCode){
	std::cout << "mult long";
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
	offset += (baseReg == 15) ? 4 : 0; //for PC as offset, remember that PC is behind PS
	int oldReg = *r[baseReg];
	switch (loadStore){
	case 0:
		*r[baseReg] += upDownBit ? offset : -offset;
		calculated = *r[baseReg];
		*r[baseReg] = writeBack ? *r[baseReg] : oldReg;
		byteFlag ? writeToAddress(calculated, *r[destinationReg]) : writeToAddress32(calculated, *r[destinationReg]);
		break;
	case 1:
		*r[baseReg] += upDownBit ? offset : -offset;
		//std::cout << "reg " << baseReg << " " << r[baseReg];
		calculated = *r[baseReg];
		*r[baseReg] = writeBack ? *r[baseReg] : oldReg;
		*r[destinationReg] = byteFlag ? loadFromAddress(calculated) : loadFromAddress32(calculated);
		break;
	}
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
		byteFlag ? writeToAddress(calculated, *r[destinationReg]) : writeToAddress32(calculated, *r[destinationReg]);
		calculated += upDownBit ? offset : -offset;
		break;
	case 1:
		*r[destinationReg] = byteFlag ? loadFromAddress(calculated) : loadFromAddress32(calculated);
		calculated += upDownBit ? offset : -offset;
		break;
	}
	*r[baseReg] = calculated;
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
			std::cout << "TBD2";
			break;

		case 1:
			*r[rn] += upDownBit ? offset : -offset;
			*r[rd] = byteFlag ? loadFromAddress(*r[rn]) : loadFromAddress32(*r[rn]);
			*r[rn] = writeBack ? *r[rn] : oldReg;
			if (debug && byteFlag)
				std::cout << "ldrb r" << rd << ", [r" << rn << " r" << rm << "] ";
			else if (debug && !byteFlag)
				std::cout << "ldr r" << rd << ", [r" << rn << " r" << rm << "] ";
			break;
	}


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
		byteFlag ? writeToAddress(*r[baseReg], *r[destinationReg]) : writeToAddress32(*r[baseReg], *r[destinationReg]);
		*r[baseReg] += upDownBit ? offset : -offset;
		break;
	case 1:
		*r[destinationReg] = byteFlag ? loadFromAddress(*r[baseReg]) : loadFromAddress32(*r[baseReg]);
		*r[baseReg] += upDownBit ? offset : -offset;
		break;
	}
}

void ARMExecute(int opCode){
    int condition = (opCode >> 28) & 0xF;
	*r[PC] += 4;
    if(ARMconditions[condition]()) //condition true
    {
        int opCodeType = (opCode >> 24) & 0xF;
        int subType;
        switch(opCodeType){
			case 15: //no interrups yet because there is no mechanism or required op codes implemented yet
				interruptController();
				std::cout << std::hex << "interrpt " << *r[15] << " ";
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
				subType = (opCode >> 20) & 15;
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
				subType = (opCode >> 20) & 0xF;
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
