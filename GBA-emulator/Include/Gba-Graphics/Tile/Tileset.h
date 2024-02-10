#pragma once

#include "Gba-Graphics/Tile/Tile.h"

class Tileset
{
public:
	Tile tileset[64][32] = {};

	void update();
	uint8_t* getTileset(bool is8bit);
	Tile::GBATile getTile(uint8_t y, uint8_t x, uint8_t palette, bool is8Bit);
	Tile::GBATile getTile(uint32_t index, uint8_t palette, bool is8Bit);
private:

	uint32_t fullTileset[64*8][32*8] = {};
};


