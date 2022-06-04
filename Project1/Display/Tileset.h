#pragma once

#include "Display/Tile.h"

class Tileset
{
public:
	Tile::GBATile tileset[64][32] = { 0 };

	Tileset();
	void update();
	uint8_t* getTileset(bool is8bit);
private:
	
	uint32_t fullTileset = { 0 };
	uint32_t manyTiles[64*8][32*8] = { 0 };
};


