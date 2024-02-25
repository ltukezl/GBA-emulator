#pragma once
#include <array>
#include <bitset>

#include "memoryAreas.h"


class TilesetMemoryObserver {
public:
	void setAccessed(const MemoryAddress address) {

	}
private:
	std::bitset<64 * 32> m_tilesetAccessed;
};