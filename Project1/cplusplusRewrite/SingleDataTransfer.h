#pragma once
#include <stdint.h>
#include "cplusplusRewrite/HwRegisters.h"

class SingleDataTransfer {
private:

	Registers& m_regs;

public:
	union {
		uint32_t val;
		struct {
			uint32_t offset : 12;
			uint32_t destinationRegister : 4;
			uint32_t baseRegister : 4;
			uint32_t loadBit : 1;
			uint32_t writeBack : 1;
			uint32_t byteTransfer : 1;
			uint32_t addOffset : 1;
			uint32_t preIndexing : 1;
			uint32_t immediateOffset : 1;
			uint32_t unused : 2;
			uint32_t executionCondition : 4;
		};
	}m_opCode;



	SingleDataTransfer(Registers& m_regs, uint32_t opCode);
	~SingleDataTransfer();

	//SingleDataTransfer(DataProcessingOpCodes opCode, DataProcessingSetOpCodes setStatus, uint32_t destReg, uint32_t firstOpReg, bool immediateFlg, uint16_t imm);

	void execute();
};