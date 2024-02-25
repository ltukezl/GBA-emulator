#pragma once
#include "memoryAreas.h"

class Sram : public IMemoryArea<Sram> {
public:
	uint8_t read8_impl(const MemoryAddress address) {
		return m_memoryArea[address.address];
	}

	uint32_t read16_impl(const MemoryAddress address) {
		uint32_t tmpResult = m_memoryArea[address.address] * 0x101;
		return RORnoCond(tmpResult, 8 * address.alignment16b());
	}

	uint32_t read32_impl(const Registers& registers, const MemoryAddress address) {
		uint32_t tmpResult = m_memoryArea[address.address] * 0x1010101;
		return RORnoCond(tmpResult, 8 * address.alignment32b());
	}

	void write8_impl(const MemoryAddress address, const uint8_t value) {
		m_memoryArea[address.address] = value;
	}

	void write16_impl(const MemoryAddress address, const uint16_t value) {
		write8(address.address, value);
		write8(address.address + 1, value);
	}

	void write32_impl(const MemoryAddress address, const uint32_t value) {
		write8(address.address, value);
		write8(address.address + 1, value);
		write8(address.address + 2, value);
		write8(address.address + 3, value);
	}

	static constexpr uint32_t m_memorySize = 0x1'0000;
	std::array<uint8_t, m_memorySize> m_memoryArea = {};
};