#pragma once
/*
class LogicOp: public Operation {
protected:
	virtual void calcConditions(uint32_t op1, uint32_t op2) = 0;
	virtual void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) = 0;

	bool isZero(uint32_t result);
	bool isNegative(int32_t result);

public:
	void execute(uint32_t& destinationRegister, uint32_t operand1, RotatorUnits& rotation, bool setConditions) override;
	LogicOp(union CPSR_t& programStatus);
};

class And : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	And(union CPSR_t& programStatus);
};

//---------

class Or : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Or(union CPSR_t& programStatus);
};

//---------

class Xor : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Xor(union CPSR_t& programStatus);
};

//---------

class Tst : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Tst(union CPSR_t& programStatus);
};

//---------

class Cmp : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Cmp(union CPSR_t& programStatus);
};

//---------

class Cmn : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Cmn(union CPSR_t& programStatus);
};

//---------

class Teq : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Teq(union CPSR_t& programStatus);
};

//---------

class Mov : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Mov(union CPSR_t& programStatus);
};

//---------

class Bic : public LogicOp {
protected:
	void calcConditions(uint32_t op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Bic(union CPSR_t& programStatus);
};

//---------

class Mvn : public LogicOp {
protected:
	void calcConditions(uint32_t Op1, uint32_t op2) override;
	void calculate(uint32_t& destinationRegister, uint32_t operand1, uint32_t operand2, bool setConditions) override;
public:
	Mvn(union CPSR_t& programStatus);
};

//---------
*/