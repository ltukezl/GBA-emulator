#include "barrelShifter.h"
#include "GBAcpu.h"

template <int ...> struct IntList {};

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
		if (sourceValue > 32){
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

class BarrelShifterDecoder {
private:
	ShiferUnit* m_shifts[4];

	union {
		uint16_t val;
		struct{
			uint16_t immediate : 8;
			uint16_t rotateAmount : 4;
		};
	}immediateRotater;

	union {
		uint16_t val;
		struct{
			uint16_t sourceRegister : 4;
			uint16_t type : 1;
			uint16_t shiftCode : 2;
			uint16_t reminder : 5;
		};
	}asd;

public:
	BarrelShifterDecoder(uint16_t immediate) {
		m_shifts[0] = new Lsl(cpsr);
		m_shifts[1] = new Lsr(cpsr);
		m_shifts[2] = new Asr(cpsr);
		m_shifts[3] = new Ror(cpsr);
		asd.val = immediate;
		immediateRotater.val = immediate;
	}

	~BarrelShifterDecoder() {
		delete m_shifts[0];
		delete m_shifts[1];
		delete m_shifts[2];
		delete m_shifts[3];

		m_shifts[0] = nullptr;
		m_shifts[1] = nullptr;
		m_shifts[2] = nullptr;
		m_shifts[3] = nullptr;
	}

	void decode(DataProcessingOpcode& opCode){
		uint32_t a = 0;
		m_shifts[asd.shiftCode]->execute(a, 0, 0, false);
	}
};

DataProcessingOpcode::DataProcessingOpcode(uint32_t opCode) {
	m_opCode.val = opCode;
}

void DataProcessingOpcode::execute() {
	//doDataProcessing(m_opCode.destinationRegister, firstOperandRegister, secondOperand)
}

void unitTestForTeppo(){
	DataProcessingOpcode(0xe3a00012).execute();
	DataProcessingOpcode(0xe3a00032).execute();
	DataProcessingOpcode(0xe3a00052).execute();
	DataProcessingOpcode(0xe3a00072).execute();
}