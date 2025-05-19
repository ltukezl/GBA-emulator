#pragma once
#include <cstdint>
#include <array>

#include "cplusplusRewrite/Shifts.h"
#include "cplusplusRewrite/barrelShifter.h"

class Registers;

enum class Rotation : uint32_t{
	LSL = 0,
	LSR = 1,
	ASR = 2,
	ROR = 3,
};

class ImmediateRotater{
public:

	struct ImmediateRotateBits
	{
		uint32_t immediate : 8;
		uint32_t shift : 4;
		uint32_t : 0;
	};

	static uint32_t calculate(Registers& regs, const uint32_t opcode, const bool setStatus);
};


class RegisterWithImmediateShifter{
public:
	static constexpr std::array<decltype(&shifts::Lsl::shift), 4> m_shifts {shifts::Lsl::shift, shifts::Lsr::shift, shifts::Asr::shift, shifts::Ror::shift};
	static constexpr std::array<decltype(&shifts::Lsl::calcConditions), 4> m_conditions{ shifts::Lsl::calcConditions, shifts::Lsr::calcConditions, shifts::Asr::calcConditions, shifts::Ror::calcConditions };

	struct registerRotateFields {
		uint32_t sourceRegister : 4;
		uint32_t type : 1;
		Rotation shiftCode : 2;
		uint32_t shiftAmount : 5;
		uint32_t : 0;
	};

	static uint32_t calculate(Registers& regs, const uint32_t opcode, bool setStatus);
};

class RegisterWithRegisterShifter{
public:
	static constexpr std::array<decltype(&shifts::Lsl::shift), 4> m_shifts{ shifts::Lsl::shift, shifts::Lsr::shift, shifts::Asr::shift, shifts::Ror::shift };
	static constexpr std::array<decltype(&shifts::Lsl::calcConditions), 4> m_conditions{ shifts::Lsl::calcConditions, shifts::Lsr::calcConditions, shifts::Asr::calcConditions, shifts::Ror::calcConditions };

	struct RegisterWithRegisterFields {
		uint32_t sourceRegister : 4;
		uint32_t type : 1;
		Rotation shiftCode : 2;
		uint32_t unused : 1;
		uint32_t shiftRegister : 5;
		uint32_t : 0;
	};

	static uint32_t calculate(Registers& regs, const uint32_t opcode, const bool setStatus);
};