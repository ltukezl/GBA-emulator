#pragma once
#include <array>
#include <bitset>
#include <iostream>

#include "memoryAreas.h"


class TilesetMemoryObserver {
public:
	void setAccessed(const MemoryAddress address) {
		m_tilesetAccessed[address.address / 32] = true;
	}

	void clearAccessed(const MemoryAddress address)
	{
		m_tilesetAccessed[address.address / 32] = false;
	}

	bool checkAccessed(const MemoryAddress address)
	{
		return m_tilesetAccessed[address.address / 32];
	}

	void setAll()
	{
		m_tilesetAccessed.set();
	}
private:
	std::bitset<64 * 32> m_tilesetAccessed;
};