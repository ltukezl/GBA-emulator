#include "conditions.h"
#include "GBAcpu.h"
#include <string>

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

int noCond(){
	return 1;
}

int(*conditions[15])() = { BEQ, BNE, BCS, BCC, BMI, BPL, BVS, BVC, BHI, BLS, BGE, BLT, BGT, BLE, noCond };
std::string conditions_s[15] = { "beq", "bne", "bcs", "bcc", "bmi", "bpl", "bvs", "bvc", "bhi", "bls", "bge", "blt", "bgt", "ble", "b" };