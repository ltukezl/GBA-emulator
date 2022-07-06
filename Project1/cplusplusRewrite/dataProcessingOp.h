#pragma once
#include <stdint.h>
#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/Shifts.h"
#include "cplusplusRewrite/operation.h"
#include "cplusplusRewrite/HwRegisters.h"

enum DataProcessingOpCodes {
	AND,
	EOR,
	SUB,
	RSB,
	ADD,
	ADC,
	SBC,
	RSC,
	TST,
	TEQ,
	CMP,
	CMN,
	ORR,
	MOV,
	BIC,
	MVN,
	LAST
};

enum DataProcessingSetOpCodes{
	NO_SET,
	SET
};

class DataProcessingOpcode {
private:
	void initialize(union CPSR& cpsr, Registers& regs);
	Registers& regs;
	Operation* tst[0x10];

public:
	union OpCodeFields{
		uint32_t val;
		struct{
			uint32_t immediate : 12;
			uint32_t destinationRegister : 4;
			uint32_t firstOperandRegister : 4;
			uint32_t setStatusCodes : 1;
			uint32_t dataProcessingOpcode : 4;
			uint32_t isImmediate : 1;
			uint32_t unused : 2;
			uint32_t executionCondition : 4;
		};
	}m_opCode;

	DataProcessingOpcode(union CPSR& cpsr, Registers& regs);
	~DataProcessingOpcode();

	static uint32_t fromFields(DataProcessingOpCodes opCode, DataProcessingSetOpCodes setStatus, uint32_t destReg, uint32_t firstOpReg, bool immediateFlg, uint16_t imm);

	void execute(uint32_t opCode);
};

void unitTestForTeppo();