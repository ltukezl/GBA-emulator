#pragma once

#include <cassert>
#include <iostream>
#include "cplusplusRewrite/HwRegisters.h"
#include "cplusplusRewrite/barrelShifter.h"
#include "GBAcpu.h"

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

static struct test regImmediateValueTests[]{
	{"mov #0"          , 0   , 0, 0x1F, LSL, { 0          , 0x4000001F }},
	{"mov #0xFF"       , 0xFF, 0, 0x1F, LSL, { 0xFF       , 0x1F }},
	{"mov #0xFF00'0000", 0xFF, 8, 0x1F, LSL, { 0xFF00'0000, 0xA000001F }},
	{"mov #0xF000'000F", 0xFF, 4, 0x1F, LSL, { 0xF000'000F, 0xA000001F }},
};

static struct test regImmediateShifterTests[]{
	{"r0 LSL #0" , 0xFFF     , 0  , 0x1F, LSL, { 0xFFF      , 0x1F }},
	{"r0 LSL #1" , 0xFFF     , 1  , 0x1F, LSL, { 0x1FFE     , 0x1F }},
	{"r0 LSL #31", 0x3       , 31 , 0x1F, LSL, { 0x8000'0000, 0xA000001F }},
	{"r0 LSR #0" , 0xFFF     , 0  , 0x1F, LSR, { 0xFFF      , 0x1F }},
	{"r0 LSR #1" , 0xFFF     , 1  , 0x1F, LSR, { 0x7FF      , 0x2000001F }},
	{"r0 LSR #31", 0xC0000000, 31 , 0x1F, LSR, { 0x1        , 0x2000001F }},
	{"r0 ASR #0" , 0x80000FFF, 0  , 0x1F, ASR, { 0x8000'0FFF, 0x8000001F }},
	{"r0 ASR #1" , 0x80000FFF, 1  , 0x1F, ASR, { 0xC000'07FF, 0xA000001F }},
	{"r0 ASR #31", 0xC0000000, 31 , 0x1F, ASR, { 0xFFFF'FFFF, 0xA000001F }},
	{"r0 ROR #1" , 0x12345678, 1  , 0x1F, ROR, { 0x091A'2B3C, 0x1F }},
	{"r0 ROR #31", 0x12345678, 31 , 0x1F, ROR, { 0x2468'ACF0, 0x1F }},
	{"r0 ROR #0" , 0x12345678, 0  , 0x1F, ROR, { 0x091A'2B3C, 0x1F }},
	{"r0 ROR #0" , 0x87654321, 0  , 0x1F, ROR, { 0x43B2'A190, 0x2000001F }},
};

static struct test regRegisterShifterTests[]{
	{"r0 LSL r1", 0 , 0xFFF     , 0x1F, LSL, { 0xFFF     , 0x1F }},
	{"r0 LSL r1", 1 , 0xFFF     , 0x1F, LSL, { 0x1FFE    , 0x1F }},
	{"r0 LSL r1", 31, 3         , 0x1F, LSL, { 0x80000000, 0xA000001F }},
	{"r0 LSL r1", 32, 3         , 0x1F, LSL, { 0         , 0x6000001F }},
	{"r0 LSL r1", 33, 3         , 0x1F, LSL, { 0         , 0x4000001F }},
	{"r0 LSL r1", -1, 3         , 0x1F, LSL, { 0         , 0x4000001F }},
	{"r0 LSR r1", 0 , 0xFFF     , 0x1F, LSR, { 0xFFF     , 0x1F }},
	{"r0 LSR r1", 1 , 0xFFF     , 0x1F, LSR, { 0x7FF     , 0x2000001F }},
	{"r0 LSR r1", 31, 0xC0000000, 0x1F, LSR, { 0x1       , 0x2000001F }},
	{"r0 LSR r1", 32, 0xC0000000, 0x1F, LSR, { 0         , 0x6000001F }},
	{"r0 LSR r1", 33, 0xC0000000, 0x1F, LSR, { 0         , 0x4000001F }},
	{"r0 LSR r1", -1, 0xC0000000, 0x1F, LSR, { 0         , 0x4000001F }},
	{"r0 ASR r1", 0 , 0x80000FFF, 0x1F, ASR, { 0x80000FFF, 0x8000001F }},
	{"r0 ASR r1", 1 , 0x80000FFF, 0x1F, ASR, { 0xC00007FF, 0xA000001F }},
	{"r0 ASR r1", 31, 0xC0000000, 0x1F, ASR, { 0xFFFFFFFF, 0xA000001F }},
	{"r0 ASR r1", 32, 0xC0000000, 0x1F, ASR, { 0xFFFFFFFF, 0xA000001F }},
	{"r0 ASR r1", 33, 0xC0000000, 0x1F, ASR, { 0xFFFFFFFF, 0xA000001F }},
	{"r0 ASR r1", -1, 0xC0000000, 0x1F, ASR, { 0xFFFFFFFF, 0xA000001F }},
	{"r0 ROR r1", 0 , 0x12345678, 0x1F, ROR, { 0x12345678, 0x1F }},
	{"r0 ROR r1", 1 , 0x12345678, 0x1F, ROR, { 0x091A2B3C, 0x1F }},
	{"r0 ROR r1", 31, 0x12345678, 0x1F, ROR, { 0x2468ACF0, 0x1F }},
	{"r0 ROR r1", 32, 0x12345678, 0x1F, ROR, { 0x12345678, 0x1F }},
	{"r0 ROR r1", 33, 0x12345678, 0x1F, ROR, { 0x091A2B3C, 0x1f }},
	{"r0 ROR r1", -1, 0x12345678, 0x1F, ROR, { 0x2468ACF0, 0x1F }},
};

static struct test PCTestsIMM[]{
	{"PC LSL #0" , 0x800120 , 0  , 0x1F, LSL, { 0x800120 , 0x1F }},
	{"PC LSL #31", 0x800120 , 31 , 0x1F, LSL, { 0        , 0x4000001F }},
	{"PC LSR #0" , 0x800120 , 0  , 0x1F, LSR, { 0x800120 , 0x1F }},
	{"PC LSR #31", 0x800120 , 31 , 0x1F, LSR, { 0        , 0x4000001F }},
	{"PC ASR #0" , 0x800120 , 0  , 0x1F, ASR, { 0x800120 , 0x1F }},
	{"PC ASR #31", 0x800120 , 31 , 0x1F, ASR, { 0        , 0x4000001F }},
};

static struct test PCTestsREG[]{
	{"PC LSL r1", 0 , 0x8000120, 0x1F, LSL, { 0x8000124, 0x1F }},
	{"PC LSR r1", 0 , 0x8000120, 0x1F, LSR, { 0x8000124, 0x1F }},
	{"PC LSR r1", 24, 0x8000120, 0x1F, LSR, { 8        , 0x1F }},
	{"PC ASR r1", 0 , 0x8000120, 0x1F, ASR, { 0x8000124, 0x1F }},
	{"PC ASR r1", 24, 0x8000120, 0x1F, ASR, { 8        , 0x1F }},
};

void loadStoreImmediateValues() {
	bool failed = false;
	for (auto& test_arr : regImmediateValueTests) {
		Registers testRegs = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		cpsr.val = test_arr.inCpsr;
		uint32_t result = ImmediateRotater(testRegs, test_arr.in0, test_arr.in1).calculate(true);
		if (result != test_arr.expected.result) {
			std::cout << std::hex << "result for test " << test_arr.name << " failed! - got: " << result << " expected: " << test_arr.expected.result << "\n" << std::dec;
			failed = true;
		}
		if (cpsr.val != test_arr.expected.cpsr) {
			std::cout << std::hex << "cpsr for test " << test_arr.name << " failed! - got: " << cpsr.val << " expected: " << test_arr.expected.cpsr << "\n\n" << std::dec;
			failed = true;
		}
	}
	assert(!failed);
}

void registerImmediateShifterTests() {
	bool failed = false;
	for (auto& test_arr : regImmediateShifterTests) {
		Registers testRegs = { test_arr.in0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		cpsr.val = test_arr.inCpsr;
		uint32_t result = RegisterWithImmediateShifter(testRegs, 0, test_arr.rotation, test_arr.in1).calculate(true);
		if (result != test_arr.expected.result) {
			std::cout << std::hex << "result for test " << test_arr.name << " failed! - got: " << result << " expected: " << test_arr.expected.result << "\n" << std::dec;
			failed = true;
		}
		if (cpsr.val != test_arr.expected.cpsr) {
			std::cout << std::hex << "cpsr for test " << test_arr.name << " failed! - got: " << cpsr.val << " expected: " << test_arr.expected.cpsr << "\n" << std::dec;
			failed = true;
		}
	}
	assert(!failed);
}

void registerRegisterShifterTests() {
	bool failed = false;
	for (auto& test_arr : regRegisterShifterTests) {
		Registers testRegs = { test_arr.in0,test_arr.in1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		cpsr.val = test_arr.inCpsr;
		uint32_t result = RegisterWithRegisterShifter(testRegs, 1, test_arr.rotation, 0).calculate(true);
		if (result != test_arr.expected.result) {
			std::cout << std::hex << "result for test " << test_arr.name << " failed! - got: " << result << " expected: " << test_arr.expected.result << "\n" << std::dec;
			failed = true;
		}
		if (cpsr.val != test_arr.expected.cpsr) {
			std::cout << std::hex << "cpsr for test " << test_arr.name << " failed! - got: " << cpsr.val << " expected: " << test_arr.expected.cpsr << "\n" << std::dec;
			failed = true;
		}
	}
	assert(!failed);
}

void PCOperandTestsIMM() {
	bool failed = false;
	for (auto& test_arr : PCTestsIMM) {
		Registers testRegs = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,test_arr.in0,0,0 };
		cpsr.val = test_arr.inCpsr;
		uint32_t result = RegisterWithImmediateShifter(testRegs, 15, test_arr.rotation, test_arr.in1).calculate(true);
		if (result != test_arr.expected.result) {
			std::cout << std::hex << "result for test " << test_arr.name << " failed! - got: " << result << " expected: " << test_arr.expected.result << "\n" << std::dec;
			failed = true;
		}
		if (cpsr.val != test_arr.expected.cpsr) {
			std::cout << std::hex << "cpsr for test " << test_arr.name << " failed! - got: " << cpsr.val << " expected: " << test_arr.expected.cpsr << "\n" << std::dec;
			failed = true;
		}
	}
	assert(!failed);
}

void PCOperandTestsREG() {
	bool failed = false;
	for (auto& test_arr : PCTestsREG) {
		Registers testRegs = { test_arr.in0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,test_arr.in1,0,0 };
		cpsr.val = test_arr.inCpsr;
		uint32_t result = RegisterWithRegisterShifter(testRegs, 15, test_arr.rotation, 0).calculate(true);
		if (result != test_arr.expected.result) {
			std::cout << std::hex << "result for test " << test_arr.name << " failed! - got: " << result << " expected: " << test_arr.expected.result << "\n" << std::dec;
			failed = true;
		}
		if (cpsr.val != test_arr.expected.cpsr) {
			std::cout << std::hex << "cpsr for test " << test_arr.name << " failed! - got: " << cpsr.val << " expected: " << test_arr.expected.cpsr << "\n" << std::dec;
			failed = true;
		}
	}
	assert(!failed);
}

void testShifter() {
	loadStoreImmediateValues();
	registerImmediateShifterTests();
	registerRegisterShifterTests();
	PCOperandTestsIMM();
	PCOperandTestsREG();
}
