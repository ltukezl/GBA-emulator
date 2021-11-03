#include <stdint.h>
#include "GBAcpu.h"

class MathOp {
protected:
	union CPSR& m_cpsr;

	virtual void calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) = 0;
	virtual void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) = 0;
public:
	void execute(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions){
		calculate(destinationRegister, operand1, operand2);
		if (setConditions)
			calcConditions(destinationRegister, operand1, operand2);
	}

	MathOp(union CPSR& programStatus) : m_cpsr(programStatus) {}
};

class Addition : public MathOp {
protected:
	void calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) override {
		m_cpsr.zero = result == 0;
		m_cpsr.negative = result < 0;
		//addCarry(operand2, operand1, saveTo);
		//addOverflow(operand2, operand1, saveTo);
	}

	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) override {
		destinationRegister = operand1 + operand2;
	}
};
