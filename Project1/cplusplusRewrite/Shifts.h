#pragma once
#include <stdint.h>

class ShiferUnit {
protected:
	union CPSR& m_cpsr;

	virtual void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) = 0;
	virtual void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) = 0;
public:
	ShiferUnit(union CPSR& programStatus) : m_cpsr(programStatus) {}
	void execute(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount, bool setStatus){
		shift(destinationRegister, sourceValue, shiftAmount);
		if (setStatus)
			calcConditions(destinationRegister, sourceValue, shiftAmount);
	};
};

class Lsl : public ShiferUnit {
protected:
	void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) override;
	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override;

public:
	Lsl(union CPSR& programStatus);
};

class Lsr : public ShiferUnit {
protected:
	void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) override;
	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override;

public:
	Lsr(union CPSR& programStatus);
};

class Asr : public ShiferUnit {
protected:
	void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) override;
	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override;

public:
	Asr(union CPSR& programStatus);
};

class Ror : public ShiferUnit {
protected:
	void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) override;
	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override;

public:
	Ror(union CPSR& programStatus);
};

class Rrx : public ShiferUnit {
protected:
	void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) override;
	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override;

public:
	Rrx(union CPSR& programStatus);
};