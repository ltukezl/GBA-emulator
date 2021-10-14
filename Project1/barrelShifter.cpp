#include "barrelShifter.h"
#include "GBAcpu.h"
#include "arithmeticOps.h"
#include "logicalOps.h"

void(*dataOperationsC[0x10])(int&, int, int) = { And, Eor, Sub, Rsb,
Add, Adc, Sbc, Rsc, Tst, Teq, Cmp, Cmn, Orr, Mov, Bic, Mvn };

class ShiferUnit {
protected:
	union CPSR& m_cpsr;

	virtual void calcConditions(uint32_t result, uint32_t sourceValue, uint8_t shiftAmount) = 0;
	virtual void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) = 0;
public:
	ShiferUnit(union CPSR& programStatus): m_cpsr(programStatus) {}
	void execute(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount, bool setStatus){
		shift(destinationRegister, sourceValue, shiftAmount);
		if (setStatus)
			calcConditions(destinationRegister, sourceValue, shiftAmount);
	};
};

class Lsl : public ShiferUnit {
protected:
	void calcConditions(uint32_t result, uint32_t sourceValue, uint8_t shiftAmount) override {
		if (shiftAmount > 32)
			m_cpsr.carry = 0;
		else if (shiftAmount > 0)
			m_cpsr.carry = ((unsigned)sourceValue >> (32 - shiftAmount) & 1);
		m_cpsr.negative = result < 0;
		m_cpsr.zero = result == 0;
	}

	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override {
		uint64_t tmp = (unsigned)sourceValue;
		destinationRegister = tmp << shiftAmount;
	}

public:
	Lsl(union CPSR& programStatus) : ShiferUnit(programStatus) {};
};

class Lsr : public ShiferUnit {
protected:
	void calcConditions(uint32_t result, uint32_t sourceValue, uint8_t shiftAmount) override {
		if (sourceValue > 0)
			m_cpsr.carry = (sourceValue >> (shiftAmount - 1) & 1);
		m_cpsr.negative = result < 0;
		m_cpsr.zero = result == 0;
	}

	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override {
		uint64_t tmp = (unsigned)sourceValue;
		destinationRegister = tmp >> sourceValue;
	}
public:
	Lsr(union CPSR& programStatus) : ShiferUnit(programStatus) {};
};

class Asr : public ShiferUnit {
protected:
	void calcConditions(uint32_t result, uint32_t sourceValue, uint8_t shiftAmount) override {
		int64_t tmp = sourceValue;
		if (sourceValue != 0)
			m_cpsr.carry = (tmp >> (sourceValue - 1) & 1);
		m_cpsr.negative = result < 0;
		m_cpsr.zero = result == 0;
	}

	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override {
		int64_t tmp = sourceValue;
		destinationRegister = tmp >> shiftAmount;
	}
public:
	Asr(union CPSR& programStatus) : ShiferUnit(programStatus) {};
};

class Ror : public ShiferUnit {
protected:
	void calcConditions(uint32_t result, uint32_t sourceValue, uint8_t shiftAmount) override {
		if (sourceValue > 0)
			m_cpsr.carry = (sourceValue >> (shiftAmount - 1) & 1);
		m_cpsr.negative = result < 0;
		m_cpsr.zero = result == 0;
	}

	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override {
		if (shiftAmount > 32){
			shift(destinationRegister, sourceValue, shiftAmount - 32);
		}
		else{
			destinationRegister = ((unsigned)sourceValue >> shiftAmount) | ((unsigned)sourceValue << (32 - shiftAmount));
		}
	}
public:
	Ror(union CPSR& programStatus) : ShiferUnit(programStatus) {};
};

class Rrx : ShiferUnit {
protected:
	void calcConditions(uint32_t result, uint32_t sourceValue, uint8_t shiftAmount) override {
		m_cpsr.carry = sourceValue & 1;
		m_cpsr.negative = result < 0;
		m_cpsr.zero = result == 0;
	}

	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override {
		destinationRegister = (m_cpsr.carry << 31) | (sourceValue >> 1);
	}
};

class RotatorUnits{
public:
	ShiferUnit* m_shifts[4];
	uint16_t m_val;

	RotatorUnits(){
		m_shifts[0] = new Lsl(cpsr);
		m_shifts[1] = new Lsr(cpsr);
		m_shifts[2] = new Asr(cpsr);
		m_shifts[3] = new Ror(cpsr);
	}
	~RotatorUnits(){
		delete m_shifts[0];
		delete m_shifts[1];
		delete m_shifts[2];
		delete m_shifts[3];

		m_shifts[0] = nullptr;
		m_shifts[1] = nullptr;
		m_shifts[2] = nullptr;
		m_shifts[3] = nullptr;
	}
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

	ImmediateRotater(uint16_t immediate) {
		immediateRotaterFields.val = immediate;
		m_val = immediate;
	}
	ImmediateRotater(uint16_t immediate, uint16_t rotateAmount) {
		immediateRotaterFields.immediate = immediate;
		immediateRotaterFields.rotateAmount = rotateAmount;
		m_val = immediateRotaterFields.val;
	}

	uint32_t calculate(bool setStatus) override{
		uint32_t tempResult = 0;
		auto ror = Ror(cpsr);
		ror.execute(tempResult, immediateRotaterFields.immediate, immediateRotaterFields.rotateAmount, setStatus);
		ror.execute(tempResult, immediateRotaterFields.immediate, immediateRotaterFields.rotateAmount, setStatus);
		return tempResult;
	}
};

class RegisterWithImmediateShifter : public RotatorUnits{
public:
	union {
		uint16_t val;
		struct{
			uint16_t sourceRegister : 4;
			uint16_t type : 1;
			uint16_t shiftCode : 2;
			uint16_t reminder : 5;
		};
	}registerRotaterFields;
	RegisterWithImmediateShifter(uint16_t val) { registerRotaterFields.val = val; }
	
	uint32_t calculate(bool setStatus) override {
		uint32_t tmpResult = 0;
		m_shifts[registerRotaterFields.shiftCode]->execute(tmpResult, *r[registerRotaterFields.sourceRegister], registerRotaterFields.reminder, setStatus);
		return tmpResult;
	}
};

class RegisterWithRegisterShifter : public RotatorUnits{
public:
	union {
		uint16_t val;
		struct{
			uint16_t sourceRegister : 4;
			uint16_t type : 1;
			uint16_t shiftCode : 2;
			uint16_t: 1;
			uint16_t shiftRegister : 5;
		};
	}registerRotaterFields;
	RegisterWithRegisterShifter(uint16_t val) { registerRotaterFields.val = val; }

	uint32_t calculate(bool setStatus) override {
		uint32_t tmpResult = 0;
		m_shifts[registerRotaterFields.shiftCode]->execute(tmpResult, *r[registerRotaterFields.sourceRegister], *r[registerRotaterFields.shiftRegister], setStatus);
		return tmpResult;
	}
};

class BarrelShifterDecoder {
public:
	BarrelShifterDecoder() {}

	RotatorUnits* decode(DataProcessingOpcode& opCode){
		if (opCode.m_opCode.isImmediate)
			return new ImmediateRotater(opCode.m_opCode.immediate);
		else if (opCode.m_opCode.immediate & 0x8)
			return new RegisterWithImmediateShifter(opCode.m_opCode.immediate);
		else
			return new RegisterWithRegisterShifter(opCode.m_opCode.immediate);
	}
};

DataProcessingOpcode::DataProcessingOpcode(uint32_t opCode) {
	m_opCode.val = opCode;
	shifter = nullptr;
}

DataProcessingOpcode::~DataProcessingOpcode(){
	delete shifter;
}

void DataProcessingOpcode::execute() {
	shifter = BarrelShifterDecoder().decode(*this);
	uint32_t secondOperand = static_cast<RotatorUnits*>(shifter)->calculate(m_opCode.setStatusCodes);

	dataOperationsC[m_opCode.dataProcessingOpcode](*r[m_opCode.destinationRegister], *r[m_opCode.firstOperandRegister], secondOperand);
}

DataProcessingOpcode::DataProcessingOpcode(DataProcessingOpCodes opCode, DataProcessingSetOpCodes setStatus, uint32_t destReg, uint32_t firstOpReg, bool immediateFlg, uint16_t imm){
	m_opCode.executionCondition = 0xE;
	m_opCode.isImmediate = immediateFlg;
	m_opCode.dataProcessingOpcode = opCode;
	m_opCode.setStatusCodes = setStatus;
	m_opCode.firstOperandRegister = firstOpReg;
	m_opCode.destinationRegister = destReg;
	m_opCode.immediate = imm;
}

void unitTestForTeppo(){
	DataProcessingOpcode(SUB, NO_SET, 0, 0, true, ImmediateRotater(1, 3).m_val).execute();
	DataProcessingOpcode(0xe3a00012).execute();
	DataProcessingOpcode(0xe3a00032).execute();
	DataProcessingOpcode(0xe3a00052).execute();
	DataProcessingOpcode(0xe3a00072).execute();
}