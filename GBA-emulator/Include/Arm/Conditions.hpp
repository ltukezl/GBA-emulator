#ifndef CONDITIONSpp_H
#define CONDITIONSpp_H

#include <array>
#include <cplusplusRewrite/HwRegisters.h>

class ConditionChecks
{
public:
	static bool BEQ(const Registers& regs)
	{
		return regs.m_cpsr.zero;
	}

	static bool BNE(const Registers& regs)
	{
		return !regs.m_cpsr.zero;
	}

	static bool BCS(const Registers& regs)
	{
		return regs.m_cpsr.carry;
	}

	static bool BCC(const Registers& regs)
	{
		return !regs.m_cpsr.carry;
	}

	static bool BMI(const Registers& regs)
	{
		return regs.m_cpsr.negative;
	}

	static bool BPL(const Registers& regs)
	{
		return !regs.m_cpsr.negative;
	}

	static bool BVS(const Registers& regs)
	{
		return regs.m_cpsr.overflow;
	}

	static bool BVC(const Registers& regs)
	{
		return !regs.m_cpsr.overflow;
	}

	static bool BHI(const Registers& regs)
	{
		return BCS(regs) && BNE(regs);
	}

	static bool BLS(const Registers& regs)
	{
		return BCC(regs) || BEQ(regs);
	}

	static bool BLT(const Registers& regs)
	{
		return BMI(regs) ^ BVS(regs);
	}

	static bool BGE(const Registers& regs)
	{
		return ((BMI(regs) && BVS(regs)) || (BVC(regs) && BPL(regs)));
	}

	static bool BGT(const Registers& regs)
	{
		return BNE(regs) && BGE(regs);
	}

	static bool BLE(const Registers& regs)
	{
		return BEQ(regs) || BLT(regs);
	}

	static bool noCond(const Registers& regs)
	{
		return true;
	}
};

static constexpr std::array conditionspp{ &ConditionChecks::BEQ, &ConditionChecks::BNE, &ConditionChecks::BCS, &ConditionChecks::BCC, &ConditionChecks::BMI, &ConditionChecks::BPL, &ConditionChecks::BVS, &ConditionChecks::BVC, &ConditionChecks::BHI, &ConditionChecks::BLS, &ConditionChecks::BGE, &ConditionChecks::BLT, &ConditionChecks::BGT, &ConditionChecks::BLE, &ConditionChecks::noCond, &ConditionChecks::noCond };
static constexpr std::array<const char*, 15> conditionspp_s = { "BEQ", "BNE", "BCS", "BCC", "BMI", "BPL", "BVS", "BVS", "BHI", "BLS", "BGE", "BLT", "BGT", "BLE", "B" };

#endif