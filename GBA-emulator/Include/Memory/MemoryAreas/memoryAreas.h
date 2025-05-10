#pragma once

#include <cstring>
#include <cstdint>
#include <array>

#include "Arm/armopcodes.h"
#include "cplusplusRewrite/HwRegisters.h"

union MemoryAddress {
	MemoryAddress(const uint32_t value) : raw(value) {}
	MemoryAddress(const uint32_t mask, const uint32_t value) : address{ value }, mask{ mask }{}

	struct {
		const uint32_t address : 24;
		const uint32_t mask : 8;
	};
	uint32_t raw;

	uint32_t alignment16b() const { return raw & 1; }
	uint32_t alignment32b() const { return raw & 3; }
	uint32_t aligned16b() const { return raw & (~1); }
	uint32_t aligned32b() const { return raw & (~3); }
	uint32_t alignedMasked16b() const { return address & (~1); }
	uint32_t alignedMasked32b() const { return address & (~3); }
	const MemoryAddress operator%(const uint32_t value) const { return MemoryAddress{ raw % value }; }
	const MemoryAddress operator-(const uint32_t value) const { return MemoryAddress{ raw - value }; }
	const MemoryAddress operator+(const uint32_t value) const { return MemoryAddress{ raw + value }; }
	const MemoryAddress operator&(const uint32_t value) const { return MemoryAddress{ mask, address & value }; }
	const void operator=(const MemoryAddress value) { raw = value.raw; }
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

	bool m_accessed = true;

	constexpr auto& operator[](const uint32_t offset) { return getMemoryPtr()[offset]; }

private:
	constexpr Derived* get() {
		return static_cast<Derived*>(this);
	};
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

