#pragma once
#include "cplusplusRewrite/operation.h"
#include "GBAcpu.h"

class MathOp: public Operation {
protected:
	virtual void calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) = 0;
	virtual void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) = 0;

public:
	void execute(uint32_t& destinationRegister, uint32_t operand1, RotatorUnits& rotation, bool setConditions) override;
	MathOp(union CPSR& programStatus);
};

class Addition : public MathOp {
protected:
	void calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) override;
public:
	Addition(union CPSR& programStatus);
};

class Substraction : public MathOp {
protected:
	void calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) override;
public:
	Substraction(union CPSR& programStatus);
};

class ReverseSubstraction : public MathOp {
protected:
	void calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) override;
public:
	ReverseSubstraction(union CPSR& programStatus);
};

class AddWithCarry : public MathOp {
protected:
	void calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) override;
public:
	AddWithCarry(union CPSR& programStatus);
};

class BorrowSubstraction : public MathOp {
protected:
	void calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) override;
public:
	BorrowSubstraction(union CPSR& programStatus);
};

class ReverseBorrowSubstraction : public MathOp {
protected:
	void calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) override;
public:
	ReverseBorrowSubstraction(union CPSR& programStatus);
};

class Negation : public MathOp {
protected:
	void calcConditions(uint32_t result, uint32_t operand1, uint32_t operand2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2) override;
public:
	Negation(union CPSR& programStatus);
};
