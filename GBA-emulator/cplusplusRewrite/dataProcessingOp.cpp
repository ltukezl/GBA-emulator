#include "cplusplusRewrite/dataProcessingOp.h"
#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/BarrelShifterDecoder.h"
#include "cplusplusRewrite/Shifts.h"
#include "cplusplusRewrite/operation.h"
#include "cplusplusRewrite/MathOps.h"
#include "cplusplusRewrite/LogicOps.h"
#include "cplusplusRewrite/HwRegisters.h"
#include <iostream>
/*
void DataProcessingOpcode::initialize(){
	tst[0] = new And(m_regs.m_cpsr);
	tst[1] = new Or(m_regs.m_cpsr);
	tst[2] = new Substraction(m_regs.m_cpsr);
	tst[3] = new ReverseSubstraction(m_regs.m_cpsr);
	tst[4] = new Addition(m_regs.m_cpsr);
	tst[5] = new AddWithCarry(m_regs.m_cpsr);
	tst[6] = new BorrowSubstraction(m_regs.m_cpsr);
	tst[7] = new ReverseBorrowSubstraction(m_regs.m_cpsr);
	tst[8] = new Tst(m_regs.m_cpsr);
	tst[9] = new Teq(m_regs.m_cpsr);
	tst[10] = new Cmp(m_regs.m_cpsr);
	tst[11] = new Cmn(m_regs.m_cpsr);
	tst[12] = new Xor(m_regs.m_cpsr);
	tst[13] = new Mov(m_regs.m_cpsr);
	tst[14] = new Bic(m_regs.m_cpsr);
	tst[15] = new Mvn(m_regs.m_cpsr);
}

DataProcessingOpcode::DataProcessingOpcode(Registers& regs) : m_regs(regs) {
	initialize();
}

DataProcessingOpcode::~DataProcessingOpcode(){
	for (int i = 0; i < 0x10; i++) {
		delete tst[i];
		tst[i] = nullptr;
	}
}


void DataProcessingOpcode::execute(uint32_t opCode) {
	m_opCode.val = opCode;
	RotatorUnits* shifter = BarrelShifterDecoder(m_regs).decode(*this);


	//{RD, RS, RM, RN} == 15, PC =+ 4 | PC =+ 8
	//if (((opCode >> 4) & 0x12FFF1) == 0x12FFF1) => branch link
	//MRS, MSR, 
	// TODO: Mieluummin näin
	// tst[m_opCode.dataProcessingOpcode]->execute(m_opCode, *shifter);

	tst[m_opCode.dataProcessingOpcode]->execute(m_regs[m_opCode.destinationRegister],
					m_regs[m_opCode.firstOperandRegister],
					*shifter, 
					m_opCode.setStatusCodes);

	delete shifter;
}

uint32_t DataProcessingOpcode::fromFields(DataProcessingOpCodes opCode, DataProcessingSetOpCodes setStatus, uint32_t destReg, uint32_t firstOpReg, bool immediateFlg, uint16_t imm){
	
	OpCodeFields opCodeFields;
	opCodeFields.executionCondition = 0xE;
	opCodeFields.isImmediate = immediateFlg;
	opCodeFields.dataProcessingOpcode = opCode;
	opCodeFields.setStatusCodes = setStatus;
	opCodeFields.firstOperandRegister = firstOpReg;
	opCodeFields.destinationRegister = destReg;
	opCodeFields.unused = 0;
	opCodeFields.immediate = imm;
	return opCodeFields.val;
}
*/