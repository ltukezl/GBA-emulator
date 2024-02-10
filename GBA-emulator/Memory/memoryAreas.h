#pragma once

#include <cstring>
#include <cstdint>
#include <array>
#include <cstring>

#include "Arm/armopcodes.h"
#include "cplusplusRewrite/HwRegisters.h"

union MemoryAddress {
	MemoryAddress(const uint32_t value) : raw(value) {}

	struct {
		const uint32_t address : 24;
		const uint32_t mask : 8;
	};
	const uint32_t raw;

	const uint32_t alignment16b() const { return raw & 1; }
	const uint32_t alignment32b() const { return raw & 3; }
	const uint32_t aligned16b() const { return raw & (~1); }
	const uint32_t aligned32b() const { return raw & (~3); }
	const uint32_t alignedMasked16b() const { return address & (~1); }
	const uint32_t alignedMasked32b() const { return address & (~3); }
	const MemoryAddress operator%(const uint32_t value) const { return MemoryAddress{ raw % value }; }
};

enum Source {
	CPU,
};

enum MemoryAreas {
	ESystemROM_L = 0,
	ESystemROM_H = 1,
	EExternalWorkRAM = 2,
	EInternalWorkRAM = 3,
	EIoRAM = 4,
	EPaletteRAM = 5,
	EVRAM = 6,
	EOAM = 7,
	EGamePak1 = 8,
	EGamePak2 = 9,
	EGamePak3 = 10,
	EGamePak4 = 11,
	EGamePak5 = 12,
	EGamePak6 = 13,
	ESRAM_L = 14,
	ESRAM_H = 15,
};

/***
* class IMemoryAccessContext {
*     virtual void consumeCycles() = 0;
* };
* 
* class DMAContext : public IMemoryAccessContext {
*	  void consumeCycles() {};
* }
* 
* class CPUContext : public IMemoryAccessContext {
*		template<uint32_t memoryAreaCycles>
*		void consumeCycles() { counter+=2; counter+=memoryAreaCycles; };
* 
*     void consumeCycles(uint8_t memoryAreaCycles) { counter+=2; counter+=memoryAreaCycles; };
* }
* 
*/


template<class Derived>
class IMemoryArea {
public:
	template<typename... T>
	auto read8(const T... params) { return get()->read8_impl(params...); }

	template<typename... T>
	auto read16(const T... params) { return get()->read16_impl(params...); }

	template<typename... T>
	auto read32(const T... params) { return get()->read32_impl(params...); }

	template<typename... T>
	auto write8(const T... params) { 
		m_accessed = true;
		get()->write8_impl(params...);
	}

	template<typename... T>
	auto write16(const T... params) {
		m_accessed = true;
		get()->write16_impl(params...);
	}

	template<typename... T>
	auto write32(const T... params) { 
		m_accessed = true;
		get()->write32_impl(params...);
	}

	void clearAccess() { m_accessed = false; }

	template<typename T>
	constexpr T& as(const uint32_t addr) { return *reinterpret_cast<T*>(get()->m_memoryArea.data() + addr); }

	constexpr uint8_t* getMemoryPtr() { return get()->m_memoryArea.data(); }

private:
	
	constexpr Derived* get() {
		return static_cast<Derived*>(this);
	};

	bool m_accessed = false;
};

class ExternalWorkRAM : public IMemoryArea<ExternalWorkRAM> {
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
	}

	void write16_impl(const MemoryAddress address, const uint16_t value) {
		auto newAddress = address % m_memorySize;
		auto& memAddr = as<uint16_t>(newAddress.aligned16b());
		memAddr = value;
	}

	void write32_impl(const MemoryAddress address, const uint32_t value) {
		auto newAddress = address % m_memorySize;
		auto& memAddr = as<uint32_t>(newAddress.aligned32b());
		memAddr = value;
	}

	static constexpr uint32_t m_memorySize = 0x4'0000;
	std::array<uint8_t, m_memorySize> m_memoryArea = {};
};

class InternalWorkRAM : public IMemoryArea<InternalWorkRAM> {
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
	}

	void write16_impl(const MemoryAddress address, const uint16_t value) {
		auto newAddress = address % m_memorySize;
		auto& memAddr = as<uint16_t>(newAddress.aligned16b());
		memAddr = value;
	}

	void write32_impl(const MemoryAddress address, const uint32_t value) {
		auto newAddress = address % m_memorySize;
		auto& memAddr = as<uint32_t>(newAddress.aligned32b());
		memAddr = value;
	}

	static constexpr uint32_t m_memorySize = 0x8000;
	std::array<uint8_t, m_memorySize> m_memoryArea = {};
};

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
	}

	void write16_impl(const MemoryAddress address, const uint16_t value) {
		auto newAddress = address % m_memorySize;
		auto& memAddr = as<uint16_t>(newAddress.aligned16b());
		memAddr = value;
	}

	void write32_impl(const MemoryAddress address, const uint32_t value) {
		auto newAddress = address % m_memorySize;
		auto& memAddr = as<uint32_t>(newAddress.aligned32b());
		memAddr = value;
	}

	static constexpr uint32_t m_memorySize = 0x400;
	std::array<uint8_t, m_memorySize> m_memoryArea = {};
};

class OAMRAM : public IMemoryArea<OAMRAM> {
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

	void write8_impl(const MemoryAddress address, const uint8_t value) {}

	void write16_impl(const MemoryAddress address, const uint16_t value) {
		auto newAddress = address % m_memorySize;
		auto& memAddr = as<uint16_t>(newAddress.aligned16b());
		memAddr = value;
	}

	void write32_impl(const MemoryAddress address, const uint32_t value) {
		auto newAddress = address % m_memorySize;
		auto& memAddr = as<uint32_t>(newAddress.aligned32b());
		memAddr = value;
	}

	static constexpr uint32_t m_memorySize = 0x400;
	std::array<uint8_t, m_memorySize> m_memoryArea = {};
};

class Sram : public IMemoryArea<Sram> {
public:
	uint8_t read8_impl(const MemoryAddress address) {
		return m_memoryArea[address.address];
	}

	uint32_t read16_impl(const MemoryAddress address) {
		uint32_t tmpResult = m_memoryArea[address.address] * 0x101;
		return RORnoCond(tmpResult, 8 * address.alignment16b());
	}

	uint32_t read32_impl(const MemoryAddress address) {
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


//class GBAMemory : public IMemoryArea {
//	
// void read8(IMemoryAccessContext context, const uint32_t address) {
//		if context not in whitelist:
//			crash()
// 
//		m_memoryArea[address & mask].read8(context);
// }
//};

