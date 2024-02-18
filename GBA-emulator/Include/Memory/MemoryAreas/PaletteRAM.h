#pragma once
#include <bitset>
#include "memoryAreas.h"

class PaletteRAM : public IMemoryArea<PaletteRAM> {
public:
	uint32_t read8_impl(const MemoryAddress address) {
		auto newAddress = address % m_memorySize;
		return m_memoryArea[newAddress.address];
	}

	uint32_t read16_impl(const MemoryAddress address) {
		auto newAddress = address % m_memorySize;
		auto tmpResult = as<uint16_t>(newAddress.aligned16b());
		return RORnoCond(tmpResult, 8 * address.alignment16b());
	}

	uint32_t read32_impl(const Registers& registers, const MemoryAddress address) {
		auto newAddress = address % m_memorySize;
		auto tmpResult = as<uint32_t>(newAddress.aligned32b());
		if (registers[15] < 0x4000)
			return tmpResult;
		return RORnoCond(tmpResult, 8 * address.alignment32b());
	}

	void write8_impl(const MemoryAddress address, const uint8_t value) {
		auto newAddress = address % m_memorySize;
		m_memoryArea[newAddress.address] = value;
		m_memoryArea[newAddress.address + 1] = value;
		m_accessedPaletteColour[newAddress.alignedMasked16b() >> 1] = 1;
	}

	void write16_impl(const MemoryAddress address, const uint16_t value) {
		auto newAddress = address % m_memorySize;
		auto& memAddr = as<uint16_t>(newAddress.aligned16b());
		memAddr = value;
		m_accessedPaletteColour[newAddress.alignedMasked16b() >> 1] = 1;
	}

	void write32_impl(const MemoryAddress address, const uint32_t value) {
		auto newAddress = address % m_memorySize;
		auto& memAddr = as<uint32_t>(newAddress.aligned32b());
		memAddr = value;
		m_accessedPaletteColour[(newAddress.alignedMasked16b() >> 1) + 0] = 1;
		m_accessedPaletteColour[(newAddress.alignedMasked16b() >> 1) + 1] = 1;
	}

	static constexpr uint32_t m_memorySize = 0x400;
	std::array<uint8_t, m_memorySize> m_memoryArea = {};
	std::bitset<512> m_accessedPaletteColour;
};
