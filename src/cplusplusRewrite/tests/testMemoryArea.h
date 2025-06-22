#pragma once
#include "Memory/MemoryAreas/memoryAreas.h"
#include <iostream>
#include <assert.h>

namespace {
	/*
	ExternalWorkRAM ewram;

	void testMemoryAddress() {
		auto address = MemoryAddress(0x1234'5678);
		assert(address.mask == 0x12);
		assert(address.address == 0x34'5678);
		assert(address.alignment16b() == 0x0);
		assert(address.alignment32b() == 0x0);
		assert(address.aligned16b() == 0x1234'5678);
		assert(address.aligned32b() == 0x1234'5678);
		assert(address.alignedMasked16b() == 0x34'5678);
		assert(address.alignedMasked32b() == 0x34'5678);
	}

	void testExternalWorkRAMasEmptyW() {

	}

	void testExternalWorkRAMasEmptyRW() {
		auto addr = MemoryAddress(0x00);
		assert(ewram.read8(addr) == 0x0);
		ewram.write8(addr, 0x32);
		assert(ewram.read8(addr) == 0x32);
	}


	void testAllMemoryArea() {
		testMemoryAddress();
		testExternalWorkRAMasEmptyRW();
	}
	*/
}