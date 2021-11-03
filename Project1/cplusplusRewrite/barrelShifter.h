#pragma once
#include <stdint.h>
#include "cplusplusRewrite/Shifts.h"
#include "cplusplusRewrite/barrelShifter.h"


enum Rotation{
	LSL,
	LSR,
	ASR,
	ROR,
};

class RotatorUnits{
public:
	ShiferUnit* m_shifts[4];
	uint16_t m_val;

	RotatorUnits();
	~RotatorUnits();
	virtual uint32_t calculate(bool setStatus) = 0;
};

class ImmediateRotater : public RotatorUnits{
public:
	union {
		uint16_t val;
		struct{
			uint16_t immediate : 8;
			uint16_t rotateAmount : 4;
		};
	}immediateRotaterFields;

	ImmediateRotater(uint16_t immediate);
	ImmediateRotater(uint16_t immediate, uint16_t rotateAmount);

	uint32_t calculate(bool setStatus) override;
};

class RegisterWithImmediateShifter : public RotatorUnits{
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

	RegisterWithImmediateShifter(uint16_t val);
	RegisterWithImmediateShifter(uint16_t sourceRegister, Rotation rotation, uint16_t shiftAmount);

	uint32_t calculate(bool setStatus) override;
};

class RegisterWithRegisterShifter : public RotatorUnits{
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
	RegisterWithRegisterShifter(uint16_t val);
	RegisterWithRegisterShifter(uint16_t sourceRegister, Rotation rotation, uint16_t shiftRegister);

	uint32_t calculate(bool setStatus) override;
};