#include "CommonOperations/conditions.h"
#include "GBAcpu.h"

int BEQ(){
	return r.m_cpsr.zero;
}

int BNE(){
	return !r.m_cpsr.zero;
}

int BCS(){
	return r.m_cpsr.carry;
}

int BCC(){
	return !r.m_cpsr.carry;
}

int BMI() {
	return r.m_cpsr.negative;
}

int BPL() {
	return !r.m_cpsr.negative;
}

int BVS() {
	return r.m_cpsr.overflow;
}

int BVC() {
	return !r.m_cpsr.overflow;
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