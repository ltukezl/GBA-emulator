#pragma once
#include <stdint.h>

class DataProcessingOpcode {

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

	void execute();
};

void unitTestForTeppo();