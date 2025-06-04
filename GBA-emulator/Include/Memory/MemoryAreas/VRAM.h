#pragma once

#include "Gba-Graphics/Tile/TilesetMemoryObserver.h"
#include "Memory/MemoryAreas/memoryAreas.h"

//1204 tests 1285 1330
class VRAM : public IMemoryArea<VRAM> {
public:
	uint32_t read8_impl(const MemoryAddress address) {
		const uint32_t memoryRegion = (address.address >> 15) & 0x3;
		auto newAddress = address % m_regionSize;
		return m_fullRegion[memoryRegion][newAddress.address];
	}

	uint32_t read16_impl(const MemoryAddress address) {
		const uint32_t memoryRegion = (address.address >> 15) & 0x3;
		auto newAddress = address % m_regionSize;
		auto tmpResult = as<uint16_t>(newAddress.aligned16b(), memoryRegion);
		return RORnoCond(tmpResult, 8 * address.alignment16b());
	}

	uint32_t read32_impl(const Registers& registers, const MemoryAddress address) {
		const uint32_t memoryRegion = (address.address >> 15) & 0x3;
		auto newAddress = address % m_regionSize;
		auto tmpResult = as<uint32_t>(newAddress.aligned32b(), memoryRegion);
		if (registers[15] < 0x4000)
			return tmpResult;
		return RORnoCond(tmpResult, 8 * address.alignment32b());
	}

	void write8_impl(const MemoryAddress address, const uint8_t value) {
		const uint32_t memoryRegion = (address.address >> 15) & 0x3;
		if (memoryRegion == 2 || memoryRegion == 3)
			return;
		auto newAddress = address % m_regionSize;
		m_fullRegion[memoryRegion][newAddress.address] = value;
		m_fullRegion[memoryRegion][newAddress.address + 1] = value;
		if(address.address < 0x8000*2)
			m_observer.setAccessed(address);
		m_observer.setAccessed(newAddress);
	}

	void write16_impl(const MemoryAddress address, const uint16_t value) {
		const uint32_t memoryRegion = (address.address >> 15) & 0x3;
		auto newAddress = address % m_regionSize;
		auto& memAddr = as<uint16_t>(newAddress.aligned16b(), memoryRegion);
		memAddr = value;
		if(address.address < 0x8000*2)
			m_observer.setAccessed(address);
		m_observer.setAccessed(newAddress);
	}

	void write32_impl(const MemoryAddress address, const uint32_t value) {
		const uint32_t memoryRegion = (address.address >> 15) & 0x3;
		auto newAddress = address % m_regionSize;
		auto& memAddr = as<uint32_t>(newAddress.aligned32b(), memoryRegion);
		memAddr = value;
		if(address.address < 0x8000*2)
			m_observer.setAccessed(address);
	}
	static constexpr uint32_t m_objMemStart = 0x1'0000;
	static constexpr uint32_t m_regionSize = 0x8000;
	static constexpr uint32_t m_memorySize = 0x18000;
	static constexpr uint32_t m_wholeMemoryMirror = 0x2'0000;
	std::array<uint8_t, m_regionSize> m_memoryArea;
	std::array<uint8_t, m_regionSize> m_bgAndTileRegion2;
	std::array<uint8_t, m_regionSize> m_objectRegion;

	std::array<uint8_t*, 4> m_fullRegion{ m_memoryArea.data() , m_bgAndTileRegion2.data(), m_objectRegion.data(), m_objectRegion.data() };
	TilesetMemoryObserver m_observer;
private:
	template <typename T>
	constexpr T& as(const uint32_t addr, const uint32_t region) { return *reinterpret_cast<T*>(m_fullRegion[region] + addr); }
};
