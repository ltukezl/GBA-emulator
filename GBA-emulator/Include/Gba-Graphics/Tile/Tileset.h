#pragma once

#include <array>
#include "Gba-Graphics/Tile/Tile.h"

class Tileset
{
public:

	union Tileset_t {
		std::array<std::array<Tile, 32>, 64> grid;
		std::array<Tile, 64*32> linear;
	} tileset = {};
	
	void update();
	uint8_t* getTileset(const bool is8bit);
	Tile::GBATile getTile(const uint8_t y, const uint8_t x, const uint8_t palette, const bool is8Bit);
	Tile::GBATile getTile(const uint32_t index, const uint8_t palette, const bool is8Bit);
private:

	uint32_t fullTileset[64*8][32*8] = {};
};


