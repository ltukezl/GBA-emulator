#include "cplusplusRewrite/dataProcessingOp.h"
#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/BarrelShifterDecoder.h"
#include "cplusplusRewrite/Shifts.h"
#include "CommonOperations/arithmeticOps.h"
#include "CommonOperations/logicalOps.h"
#include <GBAcpu.h>
#include <iostream>

void(*dataOperationsC[0x10])(int&, int, int) = { And, Eor, Sub, Rsb,
Add, Adc, Sbc, Rsc, Tst, Teq, Cmp, Cmn, Orr, Mov, Bic, Mvn };

void(*dataOperationsCCond[0x10])(int&, int, int) = { Ands, Eors, Subs, Rsbs,
Adds, Adcs, Sbcs, Rscs, Tst, Teq, Cmp, Cmn, Orrs, Movs, Bics, Mvns };

DataProcessingOpcode::DataProcessingOpcode(uint32_t opCode) {
	m_opCode.val = opCode;
	shifter = nullptr;
}

DataProcessingOpcode::~DataProcessingOpcode(){
	delete shifter;
}

void DataProcessingOpcode::execute() {
	shifter = BarrelShifterDecoder().decode(*this);
	uint32_t secondOperand = shifter->calculate(m_opCode.setStatusCodes);

	if (m_opCode.setStatusCodes)
		dataOperationsCCond[m_opCode.dataProcessingOpcode](*r[m_opCode.destinationRegister], *r[m_opCode.firstOperandRegister], secondOperand);
	else
		dataOperationsC[m_opCode.dataProcessingOpcode](*r[m_opCode.destinationRegister], *r[m_opCode.firstOperandRegister], secondOperand);
}

DataProcessingOpcode::DataProcessingOpcode(DataProcessingOpCodes opCode, DataProcessingSetOpCodes setStatus, uint32_t destReg, uint32_t firstOpReg, bool immediateFlg, uint16_t imm){
	m_opCode.executionCondition = 0xE;
	m_opCode.isImmediate = immediateFlg;
	m_opCode.dataProcessingOpcode = opCode;
	m_opCode.setStatusCodes = setStatus;
	m_opCode.firstOperandRegister = firstOpReg;
	m_opCode.destinationRegister = destReg;
	m_opCode.immediate = imm;
}

void assert(uint32_t regVal, uint32_t regExpected, uint32_t cpsrVal, uint32_t cpsrExpected, uint32_t line){
	if (regVal != regExpected || cpsrVal != cpsrExpected){
		std::cout << "Line: " << line << std::endl;
		std::cout << std::hex;
		std::cout << "register got " << regVal << " expected " << regExpected << std::endl;
		std::cout << "cpsr got " << cpsrVal << " expected " << cpsrExpected << std::endl;
		std::cout << std::dec;
	}
}

void unitTestForTeppo(){
	//mov tests
	cpsr.val = 0x1F;
	*r[0] = 0;
	DataProcessingOpcode(MOV, SET, 0, 1, true, ImmediateRotater(0, 0).m_val).execute();
	assert(*r[0], 0, cpsr.val, 0x4000001f, __LINE__);
	DataProcessingOpcode(MOV, SET, 0, 1, true, ImmediateRotater(0xFF, 0).m_val).execute();
	assert(*r[0], 0xFF, cpsr.val, 0x0000001f, __LINE__);
	DataProcessingOpcode(MOV, SET, 0, 1, true, ImmediateRotater(0xFF, 8).m_val).execute();
	assert(*r[0], 0xFF000000, cpsr.val, 0xA000001f, __LINE__);
	cpsr.val = 0x1F;
	DataProcessingOpcode(MOV, SET, 0, 1, true, ImmediateRotater(0xFF, 4).m_val).execute();
	assert(*r[0], 0xF000000F, cpsr.val, 0xA000001f, __LINE__);
	cpsr.val = 0x1F;

	//lsl tests RegisterWithImmediateShifter
	*r[1] = 0xFFF;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, LSL, 0).m_val).execute();
	assert(*r[0], 0xFFF, cpsr.val, 0x1F, __LINE__);

	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, LSL, 1).m_val).execute();
	assert(*r[0], 0x1FFE, cpsr.val, 0x1F, __LINE__);

	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, LSL, 31).m_val).execute();
	assert(*r[0], 0x80000000, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;

	//lsr tests RegisterWithImmediateShifter
	*r[1] = 0xFFF;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, LSR, 0).m_val).execute();
	assert(*r[0], 0xFFF, cpsr.val, 0x1F, __LINE__);

	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, LSR, 1).m_val).execute();
	assert(*r[0], 0x7FF, cpsr.val, 0x2000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[1] = 0xC0000000;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, LSR, 31).m_val).execute();
	assert(*r[0], 0x1, cpsr.val, 0x2000001F, __LINE__);
	cpsr.val = 0x1F;

	//asr tests RegisterWithImmediateShifter
	*r[1] = 0x80000FFF;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, ASR, 0).m_val).execute();
	assert(*r[0], 0x80000FFF, cpsr.val, 0x8000001F, __LINE__);
	cpsr.val = 0x1F;

	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, ASR, 1).m_val).execute();
	assert(*r[0], 0xC00007FF, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[1] = 0xC0000000;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, ASR, 31).m_val).execute();
	assert(*r[0], 0xFFFFFFFF, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;

	//LSL tests RegisterWithRegisterShifter
	*r[2] = 0;
	*r[1] = 0xFFF;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSL, 2).m_val).execute();
	assert(*r[0], 0xFFF, cpsr.val, 0x1F, __LINE__);

	*r[2] = 1;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSL, 2).m_val).execute();
	assert(*r[0], 0x1FFE, cpsr.val, 0x1F, __LINE__);

	*r[1] = 0x3;
	*r[2] = 31;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSL, 2).m_val).execute();
	assert(*r[0], 0x80000000, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = 32;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSL, 2).m_val).execute();
	assert(*r[0], 0, cpsr.val, 0x6000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = 33;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSL, 2).m_val).execute();
	assert(*r[0], 0, cpsr.val, 0x4000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = -1;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSL, 2).m_val).execute();
	assert(*r[0], 0, cpsr.val, 0x4000001F, __LINE__);
	cpsr.val = 0x1F;

	//lsr tests RegisterWithRegisterShifter
	*r[1] = 0xFFF;
	*r[2] = 0;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSR, 2).m_val).execute();
	assert(*r[0], 0xFFF, cpsr.val, 0x1F, __LINE__);

	*r[2] = 1;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSR, 2).m_val).execute();
	assert(*r[0], 0x7FF, cpsr.val, 0x2000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[1] = 0xC0000000;
	*r[2] = 31;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSR, 2).m_val).execute();
	assert(*r[0], 0x1, cpsr.val, 0x2000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = 32;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSR, 2).m_val).execute();
	assert(*r[0], 0, cpsr.val, 0x6000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = 33;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSR, 2).m_val).execute();
	assert(*r[0], 0, cpsr.val, 0x4000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = -1;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSR, 2).m_val).execute();
	assert(*r[0], 0, cpsr.val, 0x4000001F, __LINE__);
	cpsr.val = 0x1F;

	//ASR tests RegisterWithRegisterShifter
	*r[1] = 0x80000FFF;
	*r[2] = 0;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, ASR, 2).m_val).execute();
	assert(*r[0], 0x80000FFF, cpsr.val, 0x8000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = 1;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, ASR, 2).m_val).execute();
	assert(*r[0], 0xC00007FF, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[1] = 0xC0000000;
	*r[2] = 31;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, ASR, 2).m_val).execute();
	assert(*r[0], 0xFFFFFFFF, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = 32;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, ASR, 2).m_val).execute();
	assert(*r[0], 0xFFFFFFFF, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = 33;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, ASR, 2).m_val).execute();
	assert(*r[0], 0xFFFFFFFF, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = -1;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, ASR, 2).m_val).execute();
	assert(*r[0], 0xFFFFFFFF, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;
}