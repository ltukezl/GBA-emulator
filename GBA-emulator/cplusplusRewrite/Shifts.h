#pragma once
#include <cstdint>
#include "cplusplusRewrite/HwRegisters.h"

class ShiferUnit {
protected:
	union CPSR_t& m_cpsr;

	virtual void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) = 0;
	virtual void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) = 0;
public:
	ShiferUnit(union CPSR_t& programStatus) : m_cpsr(programStatus) {}
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
	Lsl(union CPSR_t& programStatus);
};

class Lsr : public ShiferUnit {
protected:
	void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) override;
	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override;

public:
	Lsr(union CPSR_t& programStatus);
};

class Asr : public ShiferUnit {
protected:
	void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) override;
	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override;

public:
	Asr(union CPSR_t& programStatus);
};

class Ror : public ShiferUnit {
protected:
	void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) override;
	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override;

public:
	Ror(union CPSR_t& programStatus);
};

class Rrx : public ShiferUnit {
protected:
	void calcConditions(int32_t result, uint32_t sourceValue, uint8_t shiftAmount) override;
	void shift(uint32_t& destinationRegister, uint32_t sourceValue, uint8_t shiftAmount) override;

public:
	Rrx(union CPSR_t& programStatus);
};