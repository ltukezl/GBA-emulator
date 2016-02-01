#include "iostream"
#include "GBAcpu.h"
#include "ThumbOpCodes.h"
#include "MemoryOps.h"

#define SETBIT(REG, POS) (REG |= (1 << POS))
#define ZEROBIT(REG, POS) (REG &= (~(1<< POS)))

int noCond(){
    return 1;
}

int(*ARMconditions[16])() = { BEQ, BNE, BCS, BCC, BMI, BPL, BVS, BVC, BHI, BLS, BGE, BLT, BGT, BLE, noCond};

void ARMBranch(int opCode){
    int location = opCode & 0xFFFFFF; //24 bits
	location = (location << 2) + 4;
    int m = 1U << (26 - 1); //bitextend hack
    int r = (location ^ m) - m;
    PC += r;
}

void IncrementBase(int& baseRegister, int nullParameter = 0){
    baseRegister += 4;
}

void decrementBase(int& baseRegister, int nullParameter = 0){
    baseRegister -= 4;
}

template <typename function1, typename function2>
void BlockDataTransferSave(int opCode, function1 a, function2 b){
    int baseReg = (opCode >> 16) & 15;
    int upDownBit = (opCode >> 23) & 1;
    int writeBack = (opCode >> 21) & 1;
	int regList = opCode & 0xFFFF;
    int oldBase = r[baseReg];
    if(((opCode >> 15)&1) & ~upDownBit){
        a(r[baseReg], r[15] + 12);
        b(r[baseReg], r[15] + 12);
    }
    for(int i = 0; i <15; i++){
		if (upDownBit){
			if (regList & 1){
				a(r[baseReg], r[i]);
				b(r[baseReg], r[i]);
			}
			regList >>= 1;
		}
		else if (~upDownBit){
			if (regList & 0x4000){
				a(r[baseReg], r[i]);
				b(r[baseReg], r[i]);
			}
			regList <<= 1;
		}
    }
    if(((opCode >> 15)&1) & upDownBit){
        a(r[baseReg], r[15] + 12);
        b(r[baseReg], r[15] + 12);
    }
    r[baseReg] = writeBack ? r[baseReg] : oldBase;
}

void singleDataTrasnferImmediatePre(int opCode){
    int upDownBit = (opCode >> 23) & 1;
    int byteFlag = (opCode >> 22) & 1;
    int writeBack = (opCode >> 21) & 1;
    int loadStore = (opCode >> 20) & 1;
    int baseReg = (opCode >> 16) & 15;
    int destinationReg = (opCode >> 12) & 15;
    int offset = opCode & 0xFFF;
	offset += (baseReg == 15) ? 4 : 0; //for PC as offset, remember that PC is behind
    int oldReg = r[baseReg];
    switch(loadStore){
    case 0:
        r[baseReg] += upDownBit ? offset : -offset;
		byteFlag ? writeToAddress(r[baseReg], r[destinationReg]) : writeToAddress32(r[baseReg], r[destinationReg]);
        r[baseReg] = writeBack ? r[baseReg] : oldReg;
        break;
    case 1:
        r[baseReg] += upDownBit ? offset : -offset;
		//std::cout << "reg " << baseReg << " " << r[baseReg];
		r[destinationReg] = byteFlag ? loadFromAddress(r[baseReg]) : loadFromAddress32(r[baseReg]);
        r[baseReg] = writeBack ? r[baseReg] : oldReg;
        break;
    }
}

void singleDataTrasnferImmediatePost(int opCode){
    int upDownBit = (opCode >> 23) & 1;
    int byteFlag = (opCode >> 22) & 1;
    int loadStore = (opCode >> 20) & 1;
    int baseReg = (opCode >> 16) & 15;
    int destinationReg = (opCode >> 12) & 15;
    int offset = opCode & 0xFFF;
	offset += (baseReg == 15) ? 4 : 0; //for PC as offset, remember that PC is behind
    switch(loadStore){
    case 0:
		byteFlag ? writeToAddress(r[baseReg], r[destinationReg]) : writeToAddress32(r[baseReg], r[destinationReg]);
        r[baseReg] += upDownBit ? offset : -offset;
        break;
    case 1:
		r[destinationReg] = byteFlag ? loadFromAddress(r[baseReg]) : loadFromAddress32(r[baseReg]);
        r[baseReg] += upDownBit ? offset : -offset;
        break;
    }
}

void singleDataTrasnferRegisterPre(int opCode){
    int upDownBit = (opCode >> 23) & 1;
    int byteFlag = (opCode >> 22) & 1;
    int writeBack = (opCode >> 21) & 1;
    int loadStore = (opCode >> 20) & 1;
    int baseReg = (opCode >> 16) & 15;
    int destinationReg = (opCode >> 12) & 15;
    int shiftAmount = (opCode >> 4) & 0xFF;
    int offset = r[(opCode & 15) << shiftAmount];
	offset += (baseReg == 15) ? 4 : 0; //for PC as offset, remember that PC is behind
    int oldReg = r[baseReg];
    switch(loadStore){
    case 0:
        r[baseReg] += upDownBit ? offset : -offset;
		byteFlag ? writeToAddress(r[baseReg], r[destinationReg]) : writeToAddress32(r[baseReg], r[destinationReg]);
        r[baseReg] = writeBack ? r[baseReg] : oldReg;
        break;
    case 1:
        r[baseReg] += upDownBit ? offset : -offset;
		r[destinationReg] = byteFlag ? loadFromAddress(r[baseReg]) : loadFromAddress32(r[baseReg]);
        r[baseReg] = writeBack ? r[baseReg] : oldReg;
        break;
    }
}

void singleDataTrasnferRegisterPost(int opCode){
    int upDownBit = (opCode >> 23) & 1;
    int byteFlag = (opCode >> 22) & 1;
    int loadStore = (opCode >> 20) & 1;
    int baseReg = (opCode >> 16) & 15;
    int destinationReg = (opCode >> 12) & 15;
    int shiftAmount = (opCode >> 4) & 0xFF;
    int offset = r[(opCode & 15) << shiftAmount];
	offset += (baseReg == 15) ? 4 : 0; //for PC as offset, remember that PC is behind


    switch(loadStore){
    case 0:
		byteFlag ? writeToAddress(r[baseReg], r[destinationReg]) : writeToAddress32(r[baseReg], r[destinationReg]);
        r[baseReg] += upDownBit ? offset : -offset;
        break;
    case 1:
		r[destinationReg] = byteFlag ? loadFromAddress(r[baseReg]) : loadFromAddress32(r[baseReg]);
        r[baseReg] += upDownBit ? offset : -offset;
        break;
    }
}

void singleDataSwap(int opCode){
    std::cout << "singleDataSwap\n";
}

void branchAndExhange(int opCode){
    r[15] = r[opCode & 15];
    (r[15] & 1) ? SETBIT(cprs, 5) : ZEROBIT(cprs, 5);
    r[15] = r[15] & ~-1;

}

void lslCond(int &saveTo, int from, int immidiate) {
    if(!immidiate) (saveTo >> (32 - immidiate) & 1) ? SETBIT(cprs, 30) : ZEROBIT(cprs, 30);
    saveTo = from << immidiate;
}
void lsrCond(int &saveTo, int from, int immidiate) {
    (saveTo >> (immidiate - 1) & 1) ? SETBIT(cprs, 30) : ZEROBIT(cprs, 30);
    saveTo = (unsigned)from >> immidiate;
}

void asrCond(int &saveTo, int from, int immidiate) {
    (saveTo >> ((int)immidiate - 1) & 1) ? SETBIT(cprs, 30) : ZEROBIT(cprs, 30);
    saveTo = from >> immidiate;
}

void rorCond(int &saveTo,int from, int immidiate){
    (saveTo >> (immidiate - 1) & 1) ? SETBIT(cprs, 30) : ZEROBIT(cprs, 30);
    saveTo = (from << immidiate) | (from >> (32 - immidiate));
}

void(*ARMshifts[4])(int&, int, int) = { lslCond, lsrCond, asrCond, rorCond };

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
    saveTo = operand1 + operand2 + ((cprs >> 29) & 1);
}

void ARMAdcs(int& saveTo, int operand1, int operand2){
    int tmpOperand = saveTo;
    saveTo = operand1 + operand2 + ((cprs >> 29) & 1);
    zero(saveTo);
    negative(saveTo);
    addCarry(tmpOperand, operand1, saveTo);
    addOverflow(tmpOperand, operand1, saveTo);
}

void ARMSbc(int& saveTo, int operand1, int operand2){
    saveTo = operand1 - operand2 + ((cprs >> 29) & 1) - 1;
}

void ARMSbcs(int& saveTo, int operand1, int operand2){
    int tmpOperand = saveTo;
    saveTo = operand1 - operand2 + ((cprs >> 29) & 1) - 1;
    zero(saveTo);
    negative(saveTo);
    subCarry(tmpOperand, operand1, saveTo);
    subOverflow(tmpOperand, operand1, saveTo);
}

void ARMRsc(int& saveTo, int operand1, int operand2){
    saveTo = operand2 - operand1 + ((cprs >> 29) & 1) -1;
}

void ARMRscs(int& saveTo, int operand1, int operand2){
    int tmpOperand = saveTo;
    saveTo = operand2 - operand1 + ((cprs >> 29) & 1) - 1;
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
}

void ARMCMN(int& saveTo, int operand1, int operand2){
    zero(operand1 + operand2);
    negative(operand1 + operand2);
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


void(*dataOperations[0x20])(int&, int, int) = {ARMAnd, ARMAnds, ARMEOR, ARMEORS, ARMSub, ARMSubs, ARMRsb, ARMRsbs,
        ARMAdd, ARMAdds, ARMAdc, ARMAdcs, ARMSbc, ARMSbcs, ARMRsc, ARMRscs, ARMTST, ARMTST, ARMTEQ, ARMTEQ, ARMCMP,
        ARMCMP, ARMCMN, ARMCMN, ARMORR, ARMORRS, ARMMov, ARMMovs, ARMBic, ARMBics, ARMMvn, ARMMvns};

void immediateRotate(int opCode){
    int rd = (opCode >> 12) & 15; //destination
    int rs = (opCode >> 16) & 15; //first operand
    int rn = opCode & 15; //2nd operand
    int tmpRegister = r[rn];
    int immediate = (opCode >> 7) & 0x1F;
    int shiftId = (opCode >> 5) & 3;
    ARMshifts[shiftId](tmpRegister, tmpRegister, immediate);
    int operationID = (opCode >> 20) & 0x1F;
    dataOperations[operationID](r[rd], r[rs], tmpRegister);
}

void registerRotate(int opCode){
    int rd = (opCode >> 12) & 0xF; //destination
    int rs = (opCode >> 16) & 0xF; //first operand
    int rn = opCode & 0xFF; //2nd operand
    int tmpRegister = r[rn];
    int immediate = (opCode >> 8) & 0xF;
    int shiftId = (opCode >> 5) & 3;
    ARMshifts[shiftId](tmpRegister, tmpRegister, r[immediate]);
    int operationID = (opCode >> 20) & 0x1F;
    dataOperations[operationID](r[rd], r[rs], tmpRegister);

}

void dataProcessingImmediate(int opCode){
    int rd = (opCode >> 12) & 0xF; //destination
    int rs = (opCode >> 16) & 0xF; //first operand
    int immediate = opCode  & 0xFF;
    int shift = (opCode >> 8) & 0xF;
    int operationID = (opCode >> 20) & 0x1F;
    dataOperations[operationID](r[rd], r[rs], immediate << (shift*2)); //shifts are taken by steps of 2 (undocumented?)

}

void halfDataTransfer(int opCode){
    std::cout << "half data transfer";
}

void multiply(int opCode){
    std::cout << "multiply";
}

void multiplyLong(int opCode){
    std::cout << "multiply long";
}

void ARMExecute(int opCode){
    int condition = (opCode >> 28) & 0xF;
    if(ARMconditions[condition]()) //condition true
    {
        int opCodeType = (opCode >> 24) & 0xF;
        int subType;
        switch(opCodeType){
        case 15: //no interrups yet because there is no mechanism or required op codes implemented yet
            break;
        case 14: //coProcessor data ops / register transfer, not used in GBA
            break;
        case 13: case 12: //co processor data transfer, not used in GBA
            break;
        case 11: //branch with link 
            LR = PC + 4;
            LR &= ~3; //bits 1-0 should always be cleared, but you never know
        case 10://branch 
            ARMBranch(opCode);
            break;
        case 9: //block data transfer pre offset. maybe implement S bits
            subType = (opCode >> 20) & 15;
            switch(subType){
            case 10: case 8: //writeback / no writeback, pre offset, add offset
                BlockDataTransferSave(opCode, IncrementBase, writeToAddress32);
                break;
            case 2: case 0: //writeback / no writeback, pre offset, sub offset
                BlockDataTransferSave(opCode, decrementBase, writeToAddress32);
                break;
            }
            break;
        case 8: //block data transfer post offset
            subType = (opCode >> 20) & 15;
            switch(subType){
            case 10: case 8: //writeback / no writeback, post offset, add offset
                BlockDataTransferSave(opCode, writeToAddress32, IncrementBase);
                break;
            case 2: case 0: //writeback / no writeback, post offset, sub offset
                BlockDataTransferSave(opCode, writeToAddress32 , decrementBase);
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
        case 1: case 0: //data prceossing, multiply, data transfer, branch and exhange, (hell on earth)
            if(((opCode >> 4) & 0x12FFF1) == 0x12FFF1)
                branchAndExhange(opCode);
            else if(((opCode >> 4) & 1) == 0) //data processing
                immediateRotate(opCode);
            else if(((opCode >> 7) & 1) == 0) //data processing
                registerRotate(opCode);
            else if(((opCode >> 5) & 3) > 0)
                halfDataTransfer(opCode);
            else if(((opCode >> 23) & 3) == 0)
                multiply(opCode);
            else if(((opCode >> 23) & 3) == 1)
                multiplyLong(opCode);
            else if(((opCode >> 23) & 3) == 2)
                singleDataSwap(opCode);
            break;
        }
    }
}
