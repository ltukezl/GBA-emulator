#pragma once

#include <cassert>

#include "cplusplusRewrite/HwRegisters.h"
#include "cplusplusRewrite/barrelShifter.h"
#include "GBAcpu.h"
/*
tests = [
	(0x1F, ImmediateRotater(0, 0), true, 0xFF, 0x4000001F),
	(0x1F, ImmediateRotater(0, 0), true, 0, 0x4000001F),
	(0x1F, ImmediateRotater(0xFF, 0), true, 0xFF, 0x0000001F),
	(0x1F, ImmediateRotater(0xFF, 8), true, 0xFF00'0000, 0xA000001F),
	(0x1F, ImmediateRotater(0xFF, 4), true, 0xF000'000F, 0xA000001F),
]

void loadStoreImmediateValues() {
	results = [];
	for (int i=0; i<len(tests); i++) {
		bool result = test_it(**tests[i])
		results.append(result)
	}

	if results:
		assert(false);
}


void test_it(
	uint32_t cpsr_val,
	LogicOp& op,
	bool arg,
	uint32_t expected_result,
	uint32_t expected_cpsr_val,
	) {
	union CPSR expectedCPSR = {};
	cpsr.val = cpsr_val;

	uint32_t actual_result = op.calculate(arg);
	if (actual_result != expected_result) {
		return false;
	}

	expectedCPSR.val = expected_cpsr_val;
	if (cpsr.val != expectedCPSR.val) {
		return false;
	}
}
*/
/*
struct Expected {
	uint32_t result;
	uint32_t cpsr;
};

struct test {
	char* name;
	uint32_t in0;
	uint32_t in1;
	uint32_t inCpsr;
	Rotation rotation;
	struct Expected expected;
};

static struct test immediateTests[]{
	{"...", 0, 0, 0x1F, LSL, { 0, 0x1F } },
	{"...", 0, 0, 0x1F, LSL, {0xFF, 0x4000001F}},
	{"...", 0, 0, 0x1F, LSL, {0, 0x4000001F}},
	{"...", 0xFF, 0, 0x1F, LSL, {0xFF, 0x0000001F}},
	{"...", 0xFF, 8, 0x1F, LSL, {0xFF00'0000, 0xA000001F}},
	{"...", 0xFF, 4, 0x1F, LSL, {0xF000'000F, 0xA000001F}},	
};

void asd() {
	bool failed = false;
	for (auto& test_arr : immediateTests) {
		uint32_t result = RegisterWithImmediateShifter(test_arr.in0, test_arr.rotation, test_arr.in1).calculate(true);
		if (result != test_arr.expected.result) {
			cout << "result for test" << test_arr.name << " failed!";			
			failed = true;
		}
		if (cpsr.val != test_arr.expected.cpsr) {
			cout << "cpsr for test" << test_arr.name << " failed!";
			failed = true;
		}
	}
	assert(!failed);
}
*/
void loadStoreImmediateValues() {
	uint32_t result = 0;
	union CPSR expectedCPSR;
	
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x4000001F;
	result = ImmediateRotater(0, 0).calculate(true);
	assert(result == 0);
	assert(cpsr.val == expectedCPSR.val);

	cpsr.val = 0x1F;
	expectedCPSR.val = 0x0000001F;
	result = ImmediateRotater(0xFF, 0).calculate(true);
	assert(result == 0xFF);
	assert(cpsr.val == expectedCPSR.val);

	cpsr.val = 0x1F;
	expectedCPSR.val = 0xA000001F;
	result = ImmediateRotater(0xFF, 8).calculate(true);
	assert(result == 0xFF00'0000);
	assert(cpsr.val == expectedCPSR.val);

	cpsr.val = 0x1F;
	expectedCPSR.val = 0xA000001F;
	result = ImmediateRotater(0xFF, 4).calculate(true);
	assert(result == 0xF000'000F);
	assert(cpsr.val == expectedCPSR.val);
}

void testLslImm() {
	uint32_t result = 0;
	union CPSR expectedCPSR = {};

	*r[0] = 0x0;
	*r[1] = 0xFFF;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x0000001F;
	result = RegisterWithImmediateShifter(1, LSL, 0).calculate(true);
	assert(result == 0xFFF);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 0x0;
	*r[1] = 0xFFF;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x0000001F;
	result = RegisterWithImmediateShifter(1, LSL, 1).calculate(true);
	assert(result == 0x1FFE);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 0x0;
	*r[1] = 0x3;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0xA000001F;
	result = RegisterWithImmediateShifter(1, LSL, 31).calculate(true);
	assert(result == 0x80000000);
	assert(cpsr.val == expectedCPSR.val);

	//_-------------------------------_

	*r[0] = 0;
	*r[1] = 0xFFF;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x1F;
	result = RegisterWithRegisterShifter(1, LSL, 0).calculate(true);
	assert(result == 0xFFF);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 1;
	*r[1] = 0xFFF;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x1F;
	result = RegisterWithRegisterShifter(1, LSL, 0).calculate(true);
	assert(result == 0x1FFE);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 31;
	*r[1] = 0x3;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0xA000001F;
	result = RegisterWithRegisterShifter(1, LSL, 0).calculate(true);
	assert(result == 0x80000000);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 32;
	*r[1] = 0x3;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x6000001F;
	result = RegisterWithRegisterShifter(1, LSL, 0).calculate(true);
	assert(result == 0);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 33;
	*r[1] = 0x3;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x4000001F;
	result = RegisterWithRegisterShifter(1, LSL, 0).calculate(true);
	assert(result == 0);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = -1;
	*r[1] = 0x3;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x4000001F;
	result = RegisterWithRegisterShifter(1, LSL, 0).calculate(true);
	assert(result == 0);
	assert(cpsr.val == expectedCPSR.val);
}

void testLsrImm() {
	uint32_t result = 0;
	union CPSR expectedCPSR = {};

	*r[1] = 0xFFF;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x0000001F;
	result = RegisterWithImmediateShifter(1, LSR, 0).calculate(true);
	assert(result == 0xFFF);
	assert(cpsr.val == expectedCPSR.val);

	*r[1] = 0xFFF;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x2000001F;
	result = RegisterWithImmediateShifter(1, LSR, 1).calculate(true);
	assert(result == 0x7FF);
	assert(cpsr.val == expectedCPSR.val);

	*r[1] = 0xC0000000;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x2000001F;
	result = RegisterWithImmediateShifter(1, LSR, 31).calculate(true);
	assert(result == 0x1);
	assert(cpsr.val == expectedCPSR.val);

	//_-------------------------------_

	*r[0] = 0;
	*r[1] = 0xFFF;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x1F;
	result = RegisterWithRegisterShifter(1, LSR, 0).calculate(true);
	assert(result == 0xFFF);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 1;
	*r[1] = 0xFFF;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x2000001F;
	result = RegisterWithRegisterShifter(1, LSR, 0).calculate(true);
	assert(result == 0x7FF);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 31;
	*r[1] = 0xC0000000;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x2000001F;
	result = RegisterWithRegisterShifter(1, LSR, 0).calculate(true);
	assert(result == 0x1);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 32;
	*r[1] = 0xC0000000;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x6000001F;
	result = RegisterWithRegisterShifter(1, LSR, 0).calculate(true);
	assert(result == 0);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 33;
	*r[1] = 0xC0000000;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x4000001F;
	result = RegisterWithRegisterShifter(1, LSR, 0).calculate(true);
	assert(result == 0);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = -1;
	*r[1] = 0xC0000000;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x4000001F;
	result = RegisterWithRegisterShifter(1, LSR, 0).calculate(true);
	assert(result == 0);
	assert(cpsr.val == expectedCPSR.val);
}

void testAsrImm() {
	uint32_t result = 0;
	union CPSR expectedCPSR = {};

	*r[1] = 0x80000FFF;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x8000001F;
	result = RegisterWithImmediateShifter(1, ASR, 0).calculate(true);
	assert(result == 0x80000FFF);
	assert(cpsr.val == expectedCPSR.val);

	*r[1] = 0x80000FFF;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0xA000001F;
	result = RegisterWithImmediateShifter(1, ASR, 1).calculate(true);
	assert(result == 0xC00007FF);
	assert(cpsr.val == expectedCPSR.val);

	*r[1] = 0xC0000000;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0xA000001F;
	result = RegisterWithImmediateShifter(1, ASR, 31).calculate(true);
	assert(result == 0xFFFFFFFF);
	assert(cpsr.val == expectedCPSR.val);

	//_-------------------------------_

	*r[0] = 0;
	*r[1] = 0x80000FFF;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x8000001F;
	result = RegisterWithRegisterShifter(1, ASR, 0).calculate(true);
	assert(result == 0x80000FFF);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 1;
	*r[1] = 0x80000FFF;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0xA000001F;
	result = RegisterWithRegisterShifter(1, ASR, 0).calculate(true);
	assert(result == 0xC00007FF);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 31;
	*r[1] = 0xC0000000;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0xA000001F;
	result = RegisterWithRegisterShifter(1, ASR, 0).calculate(true);
	assert(result == 0xFFFFFFFF);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 32;
	*r[1] = 0xC0000000;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0xA000001F;
	result = RegisterWithRegisterShifter(1, ASR, 0).calculate(true);
	assert(result == 0xFFFFFFFF);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 33;
	*r[1] = 0xC0000000;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0xA000001F;
	result = RegisterWithRegisterShifter(1, ASR, 0).calculate(true);
	assert(result == 0xFFFFFFFF);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = -1;
	*r[1] = 0xC0000000;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0xA000001F;
	result = RegisterWithRegisterShifter(1, ASR, 0).calculate(true);
	assert(result == 0xFFFFFFFF);
	assert(cpsr.val == expectedCPSR.val);
}

void testRorImm() {
	uint32_t result = 0;
	union CPSR expectedCPSR = {};

	*r[1] = 0x12345678;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x0000001F;
	result = RegisterWithImmediateShifter(1, ROR, 1).calculate(true);
	assert(result == 0x091A2B3C);
	assert(cpsr.val == expectedCPSR.val);

	*r[1] = 0x12345678;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x0000001F;
	result = RegisterWithImmediateShifter(1, ROR, 31).calculate(true);
	assert(result == 0x2468ACF0);
	assert(cpsr.val == expectedCPSR.val);

	//_-------------------------------_

	*r[0] = 0;
	*r[1] = 0x12345678;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x0000001F;
	result = RegisterWithRegisterShifter(1, ROR, 0).calculate(true);
	assert(result == 0x12345678);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 1;
	*r[1] = 0x12345678;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x0000001F;
	result = RegisterWithRegisterShifter(1, ROR, 0).calculate(true);
	assert(result == 0x091A2B3C);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 31;
	*r[1] = 0x12345678;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x0000001F;
	result = RegisterWithRegisterShifter(1, ROR, 0).calculate(true);
	assert(result == 0x2468ACF0);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 32;
	*r[1] = 0x12345678;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x0000001F;
	result = RegisterWithRegisterShifter(1, ROR, 0).calculate(true);
	assert(result == 0x12345678);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 33;
	*r[1] = 0x12345678;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x0000001F;
	result = RegisterWithRegisterShifter(1, ROR, 0).calculate(true);
	assert(result == 0x091A2B3C);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = -1;
	*r[1] = 0x12345678;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x0000001F;
	result = RegisterWithRegisterShifter(1, ROR, 0).calculate(true);
	assert(result == 0x2468ACF0);
	assert(cpsr.val == expectedCPSR.val);

	//---------------------------------------

	*r[1] = 0x12345678;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x0000001F;
	result = RegisterWithImmediateShifter(1, ROR, 0).calculate(true);
	assert(result == 0x091A2B3C);
	assert(cpsr.val == expectedCPSR.val);

	*r[1] = 0x87654321;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x2000001F;
	result = RegisterWithImmediateShifter(1, ROR, 0).calculate(true);
	assert(result == 0x43B2A190);
	assert(cpsr.val == expectedCPSR.val);
}

void testPCreg() {
	uint32_t result = 0;
	union CPSR expectedCPSR = {};

	*r[EProgramCounter] = 0x800120;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x1F;
	result = RegisterWithImmediateShifter(15, LSL, 0).calculate(true);
	assert(result == 0x800128);
	assert(cpsr.val == expectedCPSR.val);

	*r[EProgramCounter] = 0x800120;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x4000001F;
	result = RegisterWithImmediateShifter(15, LSL, 31).calculate(true);
	assert(result == 0);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 0;
	*r[EProgramCounter] = 0x800120;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x1F;
	result = RegisterWithRegisterShifter(15, LSL, 0).calculate(true);
	assert(result == 0x80012C);
	assert(cpsr.val == expectedCPSR.val);
	//--------------------------
	*r[EProgramCounter] = 0x800120;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x1F;
	result = RegisterWithImmediateShifter(15, LSR, 0).calculate(true);
	assert(result == 0x800128);
	assert(cpsr.val == expectedCPSR.val);

	*r[EProgramCounter] = 0x800120;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x4000001F;
	result = RegisterWithImmediateShifter(15, LSR, 31).calculate(true);
	assert(result == 0);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 0;
	*r[EProgramCounter] = 0x800120;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x1F;
	result = RegisterWithRegisterShifter(15, LSR, 0).calculate(true);
	assert(result == 0x80012C);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 24;
	*r[EProgramCounter] = 0x800120;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x1F;
	result = RegisterWithRegisterShifter(15, LSR, 0).calculate(true);
	assert(result == 8);
	assert(cpsr.val == expectedCPSR.val);

	//--------------------------

	*r[EProgramCounter] = 0x800120;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x1F;
	result = RegisterWithImmediateShifter(15, ASR, 0).calculate(true);
	assert(result == 0x800128);
	assert(cpsr.val == expectedCPSR.val);

	*r[EProgramCounter] = 0x800120;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x4000001F;
	result = RegisterWithImmediateShifter(15, ASR, 31).calculate(true);
	assert(result == 0);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 0;
	*r[EProgramCounter] = 0x800120;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x1F;
	result = RegisterWithRegisterShifter(15, ASR, 0).calculate(true);
	assert(result == 0x80012C);
	assert(cpsr.val == expectedCPSR.val);

	*r[0] = 24;
	*r[EProgramCounter] = 0x800120;
	cpsr.val = 0x1F;
	expectedCPSR.val = 0x1F;
	result = RegisterWithRegisterShifter(15, ASR, 0).calculate(true);
	assert(result == 8);
	assert(cpsr.val == expectedCPSR.val);

	//--------------------------
}

void testShifter() {
	r = usrSys;
	loadStoreImmediateValues();
	testLslImm();
	testLsrImm();
	testAsrImm();
	testRorImm();
	testPCreg();
}
