#pragma once

#include <cassert>

#include "cplusplusRewrite/HwRegisters.h"
#include "cplusplusRewrite/dataProcessingOp.h"

void testDataProcessingAndOpSunnyDay() {
    // ARRANGE
	union CPSR mockCpsr = {};
	union CPSR expectedCpsr = {};
	mockCpsr.mode = USR;
	expectedCpsr.mode = USR;
	auto mockReg = Registers({ 0xFF,0xFFFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR);
	auto DataProcessing = DataProcessingOpcode(mockCpsr, mockReg);
	auto opCode = DataProcessingOpcode::fromFields(AND, SET, 2, 1, true, ImmediateRotater(0xFF, 0).m_val);

    // ACT
	DataProcessing.execute(opCode);

    // ASSERT
	//assert(*r[0], 0, mockCpsr.val, 0x4000001f, __LINE__);
    assert(mockReg == Registers({ 0xFF,0xFFFF,0xFF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, USR));
    assert(mockCpsr.val == expectedCpsr.val);
}

void testDataprocessingPCShifts() {
    // ARRANGE
	union CPSR mockCpsr;
	mockCpsr.val = 0x1F;
	auto mockReg = Registers({ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 });
	auto DataProcessing = DataProcessingOpcode(mockCpsr, mockReg);
	auto opCode = DataProcessingOpcode::fromFields(ADD, NO_SET, 0, 15, true, ImmediateRotater(0, 0).m_val);

    // ACT
	DataProcessing.execute(opCode);

    // ASSERT
    assert(mockReg == Registers({ 0x10208,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR));
    assert(mockCpsr.val == 0x1F);
	// ----------------------------------
	// ARRANGE
	mockCpsr.val = 0x1F;
	mockReg = Registers({ 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 });
	opCode = DataProcessingOpcode::fromFields(ADD, NO_SET, 0, 15, false, RegisterWithImmediateShifter(1, LSL, 0).m_val);

	// ACT
	DataProcessing.execute(opCode);

	// ASSERT
	assert(mockReg == Registers({ 0x10209,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR));
	assert(mockCpsr.val == 0x1F);

	// ----------------------------------
	// ARRANGE
	mockCpsr.val = 0x1F;
	mockReg = Registers({ 0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 });
	opCode = DataProcessingOpcode::fromFields(ADD, NO_SET, 0, 15, false, RegisterWithImmediateShifter(1, LSL, 1).m_val);

	// ACT
	DataProcessing.execute(opCode);

	// ASSERT
	assert(mockReg == Registers({ 0x1020A,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR));
	assert(mockCpsr.val == 0x1F);

	// ----------------------------------
	// ARRANGE
	mockCpsr.val = 0x1F;
	mockReg = Registers({ 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 });
	opCode = DataProcessingOpcode::fromFields(ADD, NO_SET, 0, 15, false, RegisterWithRegisterShifter(0, LSL, 1).m_val);

	// ACT
	DataProcessing.execute(opCode);

	// ASSERT
	assert(mockReg == Registers({ 0x1020B,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR));
	assert(mockCpsr.val == 0x1F);

	// ----------------------------------
	// ARRANGE
	mockCpsr.val = 0x1F;
	mockReg = Registers({ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 });
	opCode = DataProcessingOpcode::fromFields(ADD, NO_SET, 0, 15, false, RegisterWithRegisterShifter(0, LSL, 1).m_val);

	// ACT
	DataProcessing.execute(opCode);

	// ASSERT
	assert(mockReg == Registers({ 0x1020C,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR));
	assert(mockCpsr.val == 0x1F);

	// ----------------------------------
	// ARRANGE
	mockCpsr.val = 0x1F;
	mockReg = Registers({ 1,0xA0000005,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 });
	opCode = DataProcessingOpcode::fromFields(ADD, NO_SET, 0, 15, false, RegisterWithImmediateShifter(1, ROR, 1).m_val);

	// ACT
	DataProcessing.execute(opCode);

	// ASSERT
	assert(mockReg == Registers({ 0x10208 + 0xD0000002,0xA0000005,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR));
	assert(mockCpsr.val == 0x8000001F);

	// ----------------------------------
	// ARRANGE
	mockCpsr.val = 0x1F;
	mockReg = Registers({ 1,0xA0000005,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 });
	opCode = DataProcessingOpcode::fromFields(ADD, NO_SET, 0, 15, false, RegisterWithRegisterShifter(1, ROR, 0).m_val);

	// ACT
	DataProcessing.execute(opCode);

	// ASSERT
	assert(mockReg == Registers({ 0x10208 + 0xD0000009,0xA0000005,0,0,0,0,0,0,0,0,0,0,0,0,0,0x10200,0,0 }, USR));
	assert(mockCpsr.val == 0x8000001F);
}
