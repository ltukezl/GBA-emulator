#pragma once

#include "cplusplusRewrite/tests/HwRegisterTests.h"
#include "cplusplusRewrite/tests/dataProcessingTests.h"
#include "cplusplusRewrite/tests/shifterTests.h"

void runAllTests() {
	testRegister();
	testShifter();
	dataProcessingTests();
}