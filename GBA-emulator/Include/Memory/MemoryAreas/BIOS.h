#pragma once
#include "memoryAreas.h"

class BIOS : public IMemoryArea<BIOS> {
public:
	uint32_t read8_impl(const Registers& registers, const MemoryAddress address) {
		auto newAddress = address % m_memorySize;
		if (registers[TRegisters::EProgramCounter] < 0x4000)
			return m_memoryArea[newAddress.address];
		else if (registers.getMode() != CpuModes_t::EUSR)
			return m_duringIRQ & 0xFF;
		else if (registers.getPreviousMode() == CpuModes_t::ESUPER)
			return m_afterSWI & 0xFF;
		else if (registers.getPreviousMode() == CpuModes_t::EIRQ)
			return m_afterIRQ & 0xFF;
		return m_memoryArea[newAddress.address];
	}

	uint32_t read16_impl(const Registers& registers, const MemoryAddress address) {
		auto newAddress = address % m_memorySize;
		auto tmpResult = as<uint16_t>(newAddress.aligned16b());
		if (registers[TRegisters::EProgramCounter] < 0x4000)
			tmpResult = as<uint16_t>(newAddress.aligned16b());
		else if (registers.getMode() != CpuModes_t::EUSR)
			tmpResult = m_duringIRQ & 0xFFFF;
		else if (registers.getPreviousMode() == CpuModes_t::ESUPER)
			tmpResult = m_afterSWI & 0xFFFF;
		else if (registers.getPreviousMode() == CpuModes_t::EIRQ)
			tmpResult = m_afterIRQ & 0xFFFF;

		if (registers[TRegisters::EProgramCounter] < 0x4000)
			return tmpResult;
		return RORnoCond(tmpResult, 8 * address.alignment16b());
	}

	uint32_t read32_impl(const Registers& registers, const MemoryAddress address) {
		auto newAddress = address % m_memorySize;
		auto tmpResult = 0;
		if (registers[TRegisters::EProgramCounter] < 0x4000)
			tmpResult = as<uint32_t>(newAddress.aligned32b());
		else if (registers.getMode() != CpuModes_t::EUSR)
			tmpResult = m_duringIRQ;
		else if (registers.getPreviousMode() == CpuModes_t::ESUPER)
			tmpResult = m_afterSWI;
		else if (registers.getPreviousMode() == CpuModes_t::EIRQ)
			tmpResult = m_afterIRQ;

		if (registers[TRegisters::EProgramCounter] < 0x4000)
			return tmpResult;
		return RORnoCond(tmpResult, 8 * address.alignment32b());
	}

	void write8_impl(const MemoryAddress address, const uint8_t value) {
	}

	void write16_impl(const MemoryAddress address, const uint16_t value) {
	}

	void write32_impl(const MemoryAddress address, const uint32_t value) {
	}

	static constexpr uint32_t m_memorySize = 0x4000;
	std::array<uint8_t, m_memorySize> m_memoryArea = {};

	static constexpr uint32_t m_afterSWI = 0xe3a0'2004;
	static constexpr uint32_t m_afterIRQ = 0xe510'f004;
	static constexpr uint32_t m_duringIRQ = 0xe28f'e000;
	static constexpr uint32_t m_afterStartup = 0xe129'f000;
};