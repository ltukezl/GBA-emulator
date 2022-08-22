#pragma once
#include "Display/RgbaPalette.h"
#include "Display/Tile.h"

class SpriteGenerator {
private:

	uint32_t manyTiles[32 * 8][32 * 8] = {};

public:
	Tile objTileset[32][32];

	SpriteGenerator();
	uint8_t* getTileset(bool is8bit);
	void update();
};