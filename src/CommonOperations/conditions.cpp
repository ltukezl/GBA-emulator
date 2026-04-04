#include "CommonOperations/conditions.h"
#include "GBAcpu.h"

int BEQ(const Registers& reg)
{ return reg.m_cpsr.zero; }

int BNE(const Registers& reg)
{ return !reg.m_cpsr.zero; }

int BCS(const Registers& reg)
{ return reg.m_cpsr.carry; }

int BCC(const Registers& reg)
{ return !reg.m_cpsr.carry; }

int BMI(const Registers& reg)
{ return reg.m_cpsr.negative; }

int BPL(const Registers& reg)
{ return !reg.m_cpsr.negative; }

int BVS(const Registers& reg)
{ return reg.m_cpsr.overflow; }

int BVC(const Registers& reg)
{ return !reg.m_cpsr.overflow; }

int BHI(const Registers& reg)
{ return BCS(reg) & BNE(reg); }

int BLS(const Registers& reg)
{ return BCC(reg) | BEQ(reg); }

int BLT(const Registers& reg)
{ return BMI(reg) ^ BVS(reg); }

int BGE(const Registers& reg)
{ return ((BMI(reg) & BVS(reg)) | (BVC(reg) & BPL(reg))); }

int BGT(const Registers& reg)
{ return BNE(reg) & BGE(reg); }

int BLE(const Registers& reg)
{ return BEQ(reg) | BLT(reg); }

int noCond(const Registers& reg)
{ return 1; }

int (*conditions[15])(const Registers& reg) = {BEQ, BNE, BCS, BCC, BMI,
                                               BPL, BVS, BVC, BHI, BLS,
                                               BGE, BLT, BGT, BLE, noCond};
std::string conditions_s[15] = {"beq", "bne", "bcs", "bcc", "bmi",
                                "bpl", "bvs", "bvc", "bhi", "bls",
                                "bge", "blt", "bgt", "ble", "b"};
