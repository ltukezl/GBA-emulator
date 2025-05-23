#include <bit>
#include <assert.h>
#include <format>

#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/Shifts.h"
#include "cplusplusRewrite/HwRegisters.h"

static constexpr std::array shift_strings = { "LSL", "LSR", "ASR", "ROR" };

//------------

uint32_t ImmediateRotater::calculate(Registers& registers, const uint32_t opcode, bool setStatus) {
	const auto immediateRotaterFields = std::bit_cast<ImmediateRotateBits>(opcode);
	const uint32_t tempResult = shifts::Ror::shift(immediateRotaterFields.immediate, immediateRotaterFields.shift);
	const uint32_t tempResult2 = shifts::Ror::shift(tempResult, immediateRotaterFields.shift);
	if (setStatus)
	{
		shifts::Ror::calcConditions(registers.m_cpsr, tempResult2, tempResult, immediateRotaterFields.shift);
	}
	return tempResult2;
}

std::string ImmediateRotater::disassemble(const uint32_t opcode)
{
	const auto immediateRotaterFields = std::bit_cast<ImmediateRotateBits>(opcode);
	const uint32_t tempResult = shifts::Ror::shift(immediateRotaterFields.immediate, immediateRotaterFields.shift);
	const uint32_t tempResult2 = shifts::Ror::shift(tempResult, immediateRotaterFields.shift);
	return std::format("{}", tempResult2);
}

//------------

uint32_t RegisterWithImmediateShifter::calculate(Registers& registers, const uint32_t opcode, bool setStatus) {
	const auto registerRotaterFields = std::bit_cast<registerRotateFields>(opcode);
	const bool isRrx = registerRotaterFields.shiftCode == Rotation::ROR && registerRotaterFields.shiftAmount == 0;
	uint32_t operand = registers[registerRotaterFields.sourceRegister];
	operand += (registerRotaterFields.sourceRegister == 15) ? 4 : 0;

	if (isRrx)
	{
		const uint32_t result = shifts::Rrx::shift(operand, registers.m_cpsr.carry);
		if (setStatus)
		{
			shifts::Rrx::calcConditions(registers.m_cpsr, result, operand, 1);
		}
		return result;
	}

	const uint32_t shiftAmount = (registerRotaterFields.shiftAmount == 0 && registerRotaterFields.shiftCode != Rotation::LSL) ? 0x20 : registerRotaterFields.shiftAmount;
	const uint32_t result = m_shifts[static_cast<uint32_t>(registerRotaterFields.shiftCode)](operand, shiftAmount);
	if (setStatus)
	{
		m_conditions[static_cast<uint32_t>(registerRotaterFields.shiftCode)](registers.m_cpsr, result, operand, shiftAmount);
	}
	return result;
}

std::string RegisterWithImmediateShifter::disassemble(const uint32_t opcode)
{
	const auto op = std::bit_cast<registerRotateFields>(opcode);
	const bool isRrx = op.shiftCode == Rotation::ROR && op.shiftAmount == 0;
	if (isRrx)
	{
		return std::format(", R{}, RRX", op.sourceRegister);
	}
	return std::format(", R{}, {} {}", op.sourceRegister, shift_strings[static_cast<uint32_t>(op.shiftCode)], op.shiftAmount);
}

//------------
uint32_t RegisterWithRegisterShifter::calculate(Registers& registers, const uint32_t opcode, const bool setStatus) {
	const auto registerRotateFields = std::bit_cast<RegisterWithRegisterFields>(opcode);
	uint32_t operand = registers[registerRotateFields.sourceRegister];
	operand += (registerRotateFields.sourceRegister == 15) ? 4 : 0;
	
	const uint32_t shiftAmount = registers[registerRotateFields.shiftRegister];
	const uint32_t result = m_shifts[static_cast<uint32_t>(registerRotateFields.shiftCode)](operand, shiftAmount);
	if (setStatus)
	{
		m_conditions[static_cast<uint32_t>(registerRotateFields.shiftCode)](registers.m_cpsr, result, operand, shiftAmount);
	}
	
	return result;
}

std::string RegisterWithRegisterShifter::disassemble(const uint32_t opcode)
{
	const auto op = std::bit_cast<RegisterWithRegisterFields>(opcode);
	return std::format(", R{}, {} {}", op.sourceRegister, shift_strings[static_cast<uint32_t>(op.shiftCode)], op.shiftRegister);
}
