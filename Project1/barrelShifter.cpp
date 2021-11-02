#include "barrelShifter.h"
#include "GBAcpu.h"
#include "arithmeticOps.h"
#include "logicalOps.h"
#include <iostream>

void(*dataOperationsC[0x10])(int&, int, int) = { And, Eor, Sub, Rsb,
Add, Adc, Sbc, Rsc, Tst, Teq, Cmp, Cmn, Orr, Mov, Bic, Mvn };

void(*dataOperationsCCond[0x10])(int&, int, int) = { Ands, Eors, Subs, Rsbs,
Adds, Adcs, Sbcs, Rscs, Tst, Teq, Cmp, Cmn, Orrs, Movs, Bics, Mvns };

class ShiferUnit {
protected:
	union CPSR& m_cpsr;

	virtual void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) = 0;
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
	void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) override {
		if (shiftAmount > 32)
			m_cpsr.carry = 0;
		else if (shiftAmount > 0)
			m_cpsr.carry = ((unsigned)sourceValue >> (32 - shiftAmount) & 1);
		m_cpsr.negative = result < 0;
		m_cpsr.zero = result == 0;
	}

	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override {
		uint64_t tmp = sourceValue;
		destinationRegister = tmp << shiftAmount;
	}

public:
	Lsl(union CPSR& programStatus) : ShiferUnit(programStatus) {};
};

class Lsr : public ShiferUnit {
protected:
	void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) override {
		uint64_t tmp = sourceValue;
		if (shiftAmount > 0)
			m_cpsr.carry = (tmp >> (shiftAmount - 1) & 1);
		m_cpsr.negative = result < 0;
		m_cpsr.zero = result == 0;
	}

	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override {
		uint64_t tmp = sourceValue;
		destinationRegister = tmp >> shiftAmount;
	}
public:
	Lsr(union CPSR& programStatus) : ShiferUnit(programStatus) {};
};

class Asr : public ShiferUnit {
protected:
	void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) override {
		int64_t tmp = (signed)sourceValue;
		if (shiftAmount != 0)
			m_cpsr.carry = (tmp >> (shiftAmount - 1) & 1);
		m_cpsr.negative = result < 0;
		m_cpsr.zero = result == 0;
	}

	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override {
		int64_t tmp = (signed)sourceValue;
		destinationRegister = tmp >> shiftAmount;
	}
public:
	Asr(union CPSR& programStatus) : ShiferUnit(programStatus) {};
};

class Ror : public ShiferUnit {
protected:
	void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) override {
		if (shiftAmount > 0)
			m_cpsr.carry = (sourceValue >> (shiftAmount - 1) & 1);
		m_cpsr.negative = result < 0;
		m_cpsr.zero = result == 0;
	}

	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override {
		if (shiftAmount > 32){
			shift(destinationRegister, sourceValue, shiftAmount - 32);
		}
		else{
			destinationRegister = (sourceValue >> shiftAmount) | (sourceValue << (32 - shiftAmount));
		}
	}
public:
	Ror(union CPSR& programStatus) : ShiferUnit(programStatus) {};
};

class Rrx : ShiferUnit {
protected:
	void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) override {
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
			uint16_t shiftAmount : 5;
		};
	}registerRotaterFields;
	RegisterWithImmediateShifter(uint16_t val) { registerRotaterFields.val = val; m_val = val; }
	RegisterWithImmediateShifter(uint16_t sourceRegister, Rotation rotation, uint16_t shiftAmount) {
		registerRotaterFields.sourceRegister = sourceRegister;
		registerRotaterFields.type = 0;
		registerRotaterFields.shiftCode = rotation;
		registerRotaterFields.shiftAmount = shiftAmount;
		m_val = registerRotaterFields.val;
	}
	
	uint32_t calculate(bool setStatus) override {
		uint32_t tmpResult = 0;
		m_shifts[registerRotaterFields.shiftCode]->execute(tmpResult, *r[registerRotaterFields.sourceRegister], registerRotaterFields.shiftAmount, setStatus);
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
	RegisterWithRegisterShifter(uint16_t val) { registerRotaterFields.val = val; m_val = val; }
	RegisterWithRegisterShifter(uint16_t sourceRegister, Rotation rotation, uint16_t shiftRegister) {
		registerRotaterFields.sourceRegister = sourceRegister;
		registerRotaterFields.type = 1;
		registerRotaterFields.shiftCode = rotation;
		registerRotaterFields.shiftRegister = shiftRegister;
		m_val = registerRotaterFields.val;
	}

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
		else if (opCode.m_opCode.immediate & 0x10)
			return new RegisterWithRegisterShifter(opCode.m_opCode.immediate);
		else
			return new RegisterWithImmediateShifter(opCode.m_opCode.immediate);
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

	if (m_opCode.setStatusCodes)
		dataOperationsCCond[m_opCode.dataProcessingOpcode](*r[m_opCode.destinationRegister], *r[m_opCode.firstOperandRegister], secondOperand);
	else
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

void assert(uint32_t regVal, uint32_t regExpected, uint32_t cpsrVal, uint32_t cpsrExpected, uint32_t line){
	if (regVal != regExpected || cpsrVal != cpsrExpected){
		std::cout << "Line: " << line << std::endl;
		std::cout << std::hex;
		std::cout << "register got " << regVal << " expected " << regExpected << std::endl;
		std::cout << "cpsr got " << cpsrVal << " expected " << cpsrExpected << std::endl;
		std::cout << std::dec;
	}
}

void unitTestForTeppo(){
	//mov tests
	cpsr.val = 0x1F;
	*r[0] = 0;
	DataProcessingOpcode(MOV, SET, 0, 1, true, ImmediateRotater(0, 0).m_val).execute();
	assert(*r[0], 0, cpsr.val, 0x4000001f, __LINE__);
	DataProcessingOpcode(MOV, SET, 0, 1, true, ImmediateRotater(0xFF, 0).m_val).execute();
	assert(*r[0], 0xFF, cpsr.val, 0x0000001f, __LINE__);
	DataProcessingOpcode(MOV, SET, 0, 1, true, ImmediateRotater(0xFF, 8).m_val).execute();
	assert(*r[0], 0xFF000000, cpsr.val, 0xA000001f, __LINE__);
	cpsr.val = 0x1F;
	DataProcessingOpcode(MOV, SET, 0, 1, true, ImmediateRotater(0xFF, 4).m_val).execute();
	assert(*r[0], 0xF000000F, cpsr.val, 0xA000001f, __LINE__);
	cpsr.val = 0x1F;

	//lsl tests RegisterWithImmediateShifter
	*r[1] = 0xFFF;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, LSL, 0).m_val).execute();
	assert(*r[0], 0xFFF, cpsr.val, 0x1F, __LINE__);

	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, LSL, 1).m_val).execute();
	assert(*r[0], 0x1FFE, cpsr.val, 0x1F, __LINE__);

	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, LSL, 31).m_val).execute();
	assert(*r[0], 0x80000000, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;

	//lsr tests RegisterWithImmediateShifter
	*r[1] = 0xFFF;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, LSR, 0).m_val).execute();
	assert(*r[0], 0xFFF, cpsr.val, 0x1F, __LINE__);

	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, LSR, 1).m_val).execute();
	assert(*r[0], 0x7FF, cpsr.val, 0x2000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[1] = 0xC0000000;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, LSR, 31).m_val).execute();
	assert(*r[0], 0x1, cpsr.val, 0x2000001F, __LINE__);
	cpsr.val = 0x1F;

	//asr tests RegisterWithImmediateShifter
	*r[1] = 0x80000FFF;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, ASR, 0).m_val).execute();
	assert(*r[0], 0x80000FFF, cpsr.val, 0x8000001F, __LINE__);
	cpsr.val = 0x1F;

	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, ASR, 1).m_val).execute();
	assert(*r[0], 0xC00007FF, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[1] = 0xC0000000;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithImmediateShifter(1, ASR, 31).m_val).execute();
	assert(*r[0], 0xFFFFFFFF, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;

	//LSL tests RegisterWithRegisterShifter
	*r[2] = 0;
	*r[1] = 0xFFF;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSL, 2).m_val).execute();
	assert(*r[0], 0xFFF, cpsr.val, 0x1F, __LINE__);

	*r[2] = 1;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSL, 2).m_val).execute();
	assert(*r[0], 0x1FFE, cpsr.val, 0x1F, __LINE__);

	*r[1] = 0x3;
	*r[2] = 31;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSL, 2).m_val).execute();
	assert(*r[0], 0x80000000, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = 32;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSL, 2).m_val).execute();
	assert(*r[0], 0, cpsr.val, 0x6000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = 33;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSL, 2).m_val).execute();
	assert(*r[0], 0, cpsr.val, 0x4000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = -1;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSL, 2).m_val).execute();
	assert(*r[0], 0, cpsr.val, 0x4000001F, __LINE__);
	cpsr.val = 0x1F;

	//lsr tests RegisterWithRegisterShifter
	*r[1] = 0xFFF;
	*r[2] = 0;	
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSR, 2).m_val).execute();
	assert(*r[0], 0xFFF, cpsr.val, 0x1F, __LINE__);

	*r[2] = 1;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSR, 2).m_val).execute();
	assert(*r[0], 0x7FF, cpsr.val, 0x2000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[1] = 0xC0000000;
	*r[2] = 31;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSR, 2).m_val).execute();
	assert(*r[0], 0x1, cpsr.val, 0x2000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = 32;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSR, 2).m_val).execute();
	assert(*r[0], 0, cpsr.val, 0x6000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = 33;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSR, 2).m_val).execute();
	assert(*r[0], 0, cpsr.val, 0x4000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = -1;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, LSR, 2).m_val).execute();
	assert(*r[0], 0, cpsr.val, 0x4000001F, __LINE__);
	cpsr.val = 0x1F;
	 
	//ASR tests RegisterWithRegisterShifter
	*r[1] = 0x80000FFF;
	*r[2] = 0;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, ASR, 2).m_val).execute();
	assert(*r[0], 0x80000FFF, cpsr.val, 0x8000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = 1;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, ASR, 2).m_val).execute();
	assert(*r[0], 0xC00007FF, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[1] = 0xC0000000;
	*r[2] = 31;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, ASR, 2).m_val).execute();
	assert(*r[0], 0xFFFFFFFF, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = 32;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, ASR, 2).m_val).execute();
	assert(*r[0], 0xFFFFFFFF, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = 33;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, ASR, 2).m_val).execute();
	assert(*r[0], 0xFFFFFFFF, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;

	*r[2] = -1;
	DataProcessingOpcode(MOV, SET, 0, 1, false, RegisterWithRegisterShifter(1, ASR, 2).m_val).execute();
	assert(*r[0], 0xFFFFFFFF, cpsr.val, 0xA000001F, __LINE__);
	cpsr.val = 0x1F;
}