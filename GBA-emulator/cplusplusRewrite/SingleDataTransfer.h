#pragma once
#include <cstdint>
#include "cplusplusRewrite/HwRegisters.h"

/**
 * @brief 
 * 
 * 
 */
class SingleDataTransfer {
private:

	Registers& m_regs;

public:
	union {
		uint32_t val;
		struct {
			uint32_t offset : 12; //barrel shifter
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

	//loadbit
	//ldr r0, [0x0] 
	//str [0x0], r0

	//byteTrasfer
	//ldrb r0, [0x0] 
	//strb [0x0], r0

	//writeback
	// str r1, [r2,r4]! => [r2+r4], r2 = r2+r4

	//immediateOffset
	// str r1, [r2], 0x20 => [r2+0x20]

	//LDREQB R1, [R2, R3]! 

	SingleDataTransfer(Registers& m_regs, uint32_t opCode);

	//SingleDataTransfer(DataProcessingOpCodes opCode, DataProcessingSetOpCodes setStatus, uint32_t destReg, uint32_t firstOpReg, bool immediateFlg, uint16_t imm);

	void execute();
};