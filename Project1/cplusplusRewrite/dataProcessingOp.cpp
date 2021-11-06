#include "cplusplusRewrite/dataProcessingOp.h"
#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/BarrelShifterDecoder.h"
#include "cplusplusRewrite/Shifts.h"
#include "cplusplusRewrite/operation.h"
#include "cplusplusRewrite/MathOps.h"
#include "cplusplusRewrite/LogicOps.h"
#include <GBAcpu.h>
#include <iostream>

void DataProcessingOpcode::initialize(){
	tst[0] = new And(cpsr);
	tst[1] = new Or(cpsr);
	tst[2] = new Substraction(cpsr);
	tst[3] = new ReverseSubstraction(cpsr);
	tst[4] = new Addition(cpsr);
	tst[5] = new AddWithCarry(cpsr);
	tst[6] = new BorrowSubstraction(cpsr);
	tst[7] = new ReverseBorrowSubstraction(cpsr);
	tst[8] = new Tst(cpsr);
	tst[9] = new Teq(cpsr);
	tst[10] = new Cmp(cpsr);
	tst[11] = new Cmn(cpsr);
	tst[12] = new Xor(cpsr);
	tst[13] = new Mov(cpsr);
	tst[14] = new Bic(cpsr);
	tst[15] = new Mvn(cpsr);
}

DataProcessingOpcode::DataProcessingOpcode(uint32_t opCode) {
	m_opCode.val = opCode;
	initialize();
}

DataProcessingOpcode::~DataProcessingOpcode(){
	for (int i = 0; i < 0x10; i++){
		delete tst[i];
		tst[i] = nullptr;
	}
}

void DataProcessingOpcode::execute() {
	RotatorUnits* shifter = BarrelShifterDecoder().decode(*this);

	tst[m_opCode.dataProcessingOpcode]->execute((uint32_t&)*r[m_opCode.destinationRegister],
												*r[m_opCode.firstOperandRegister],
												*shifter, 
												m_opCode.setStatusCodes);

	delete shifter;
}

DataProcessingOpcode::DataProcessingOpcode(DataProcessingOpCodes opCode, DataProcessingSetOpCodes setStatus, uint32_t destReg, uint32_t firstOpReg, bool immediateFlg, uint16_t imm){
	m_opCode.executionCondition = 0xE;
	m_opCode.isImmediate = immediateFlg;
	m_opCode.dataProcessingOpcode = opCode;
	m_opCode.setStatusCodes = setStatus;
	m_opCode.firstOperandRegister = firstOpReg;
	m_opCode.destinationRegister = destReg;
	m_opCode.unused = 0;
	m_opCode.immediate = imm;

	initialize();
}

void assert(uint32_t regVal, uint32_t regExpected, uint32_t cpsrVal, uint32_t cpsrExpected, uint32_t line){
	if (regVal != regExpected || cpsrVal != cpsrExpected){
		std::cout << std::endl;
		std::cout << "Line: " << line << std::endl;
		std::cout << std::hex;
		std::cout << "register got " << regVal << " expected " << regExpected << std::endl;
		std::cout << "cpsr got " << cpsrVal << " expected " << cpsrExpected << std::endl;
		std::cout << std::dec;
	}
}
/*
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
*/

uint32_t regs[] = {0,1,5,6,13,14,15};
uint32_t randomVals[] = {0, 0xFFFFFFFF, 2, 0xC0000000, 0x34894dd, 0x382, 0x20, 0x19};

#include "Arm/armopcodes.h"
/*
void unitTestForTeppo() {
	debug = true;
	for (uint8_t op = AND; op != LAST; op++)
	for (auto reg1 : regs)
	for (auto reg2 : regs)
	for (auto rand1 : randomVals)
	for (auto rand2 : randomVals)
	{
		*r[reg1] = rand1;
		*r[reg2] = rand2;
		cpsr.val = 0x1f;
		auto c = DataProcessingOpcode(static_cast<DataProcessingOpCodes>(op), SET, reg1, reg2, true, ImmediateRotater(rand1, rand2).m_val);
		ARMExecute(c.m_opCode.val);
		std::cout << std::endl;
		uint32_t expectedReg = *r[reg1];
		uint32_t expectedCpsr = cpsr.val;

		*r[reg1] = rand1;
		*r[reg2] = rand2;
		cpsr.val = 0x1f;
		c.execute();
		assert(*r[reg1], expectedReg, cpsr.val, expectedCpsr, 0);

		*r[reg1] = rand1;
		*r[reg2] = rand2;
		cpsr.val = 0x1f;
		cpsr.carry = 1;
		ARMExecute(c.m_opCode.val);
		std::cout << std::endl;
		expectedReg = *r[reg1];
		expectedCpsr = cpsr.val;

		*r[reg1] = rand1;
		*r[reg2] = rand2;
		cpsr.val = 0x1f;
		cpsr.carry = 1;
		c.execute();
		assert(*r[reg1], expectedReg, cpsr.val, expectedCpsr, 1);

		auto c2 = DataProcessingOpcode(static_cast<DataProcessingOpCodes>(op), NO_SET, reg1, reg2, true, ImmediateRotater(rand1, rand2).m_val);
		*r[reg1] = rand1;
		*r[reg2] = rand2;
		cpsr.val = 0x1f;
		cpsr.carry = 1;
		ARMExecute(c2.m_opCode.val);
		std::cout << std::endl;
		expectedReg = *r[reg1];
		expectedCpsr = cpsr.val;

		*r[reg1] = rand1;
		*r[reg2] = rand2;
		cpsr.val = 0x1f;
		cpsr.carry = 1;
		c2.execute();
		assert(*r[reg1], expectedReg, cpsr.val, expectedCpsr, 2);

	}
}
*/

void unitTestForTeppo() {
	auto c2 = DataProcessingOpcode(CMP, NO_SET, 0, 15, false, 0);
	c2.execute();
	ARMExecute(c2.m_opCode.val);
}