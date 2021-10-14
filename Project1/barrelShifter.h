#pragma once
#include <stdint.h>

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
};

enum DataProcessingSetOpCodes{
	NO_SET,
	SET
};

class DataProcessingOpcode {
private:
	void* shifter;

public:
	union {
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

	DataProcessingOpcode(uint32_t opCode);
	~DataProcessingOpcode();

	DataProcessingOpcode(DataProcessingOpCodes opCode, DataProcessingSetOpCodes setStatus, uint32_t destReg, uint32_t firstOpReg, bool immediateFlg, uint32_t imm);

	void execute();
};

void unitTestForTeppo();