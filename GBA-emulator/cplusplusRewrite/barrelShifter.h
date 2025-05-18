#pragma once
#include <cstdint>
#include "cplusplusRewrite/Shifts.h"
#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/HwRegisters.h"


enum Rotation : uint16_t{
	LSL = 0,
	LSR = 1,
	ASR = 2,
	ROR = 3,
	RRX = 4,
};

class ImmediateRotater{
public:

	struct ImmediateRotateBits
	{
		uint32_t immediate : 8;
		uint32_t shift : 4;
		uint32_t : 0;
	};

	static uint32_t calculate(const uint32_t opcode, CPSR_t& cpsr, const bool setStatus);
};

/*
class RegisterWithImmediateShifter{
public:
	union {
		uint16_t val;
		struct{
			uint16_t sourceRegister : 4;
			uint16_t type : 1;
			uint16_t shiftCode : 2;
			uint16_t shiftAmount : 5;
		};
	}registerRotaterFields;

	~RegisterWithImmediateShifter() override;

	RegisterWithImmediateShifter(Registers& registers, uint16_t val);
	RegisterWithImmediateShifter(Registers& registers, uint16_t sourceRegister, Rotation rotation, uint16_t shiftAmount);

	uint16_t shifter() const { return ((registerRotaterFields.shiftCode == ROR) && (registerRotaterFields.shiftAmount == 0)) ? RRX : registerRotaterFields.shiftCode; }
	uint32_t calculate(bool setStatus) override;
};

class RegisterWithRegisterShifter{
public:
	union {
		uint16_t val;
		struct{
			uint16_t sourceRegister : 4;
			uint16_t type : 1;
			uint16_t shiftCode : 2;
			uint16_t unused : 1;
			uint16_t shiftRegister : 5;
		};
	}registerRotaterFields;
	
	RegisterWithRegisterShifter(Registers& registers, uint16_t val);
	RegisterWithRegisterShifter(Registers& registers, uint16_t sourceRegister, Rotation rotation, uint16_t shiftRegister);
	~RegisterWithRegisterShifter() override ;

	uint32_t calculate(bool setStatus) override;
};

*/