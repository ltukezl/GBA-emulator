#pragma once
#include "cplusplusRewrite/operation.h"
#include "GBAcpu.h"

class LogicOp: public Operation {
protected:
	virtual void calcConditions(uint32_t op1, uint32_t op2) = 0;
	virtual void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) = 0;

	bool isZero(uint32_t result);
	bool isNegative(int32_t result);

public:
	void execute(uint32_t& destinationRegister, uint32_t operand1, RotatorUnits& rotation, bool setConditions) override;
	LogicOp(union CPSR& programStatus);
};

class And : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	And(union CPSR& programStatus);
};

//---------

class Or : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Or(union CPSR& programStatus);
};

//---------

class Xor : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Xor(union CPSR& programStatus);
};

//---------

class Tst : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Tst(union CPSR& programStatus);
};

//---------

class Cmp : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Cmp(union CPSR& programStatus);
};

//---------

class Cmn : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Cmn(union CPSR& programStatus);
};

//---------

class Teq : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Teq(union CPSR& programStatus);
};

//---------

class Mov : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Mov(union CPSR& programStatus);
};

//---------

class Bic : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Bic(union CPSR& programStatus);
};

//---------

class Mvn : public LogicOp {
protected:
	void calcConditions(uint32_t Op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Mvn(union CPSR& programStatus);
};

//---------