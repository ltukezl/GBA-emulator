#pragma once
#include <vector>
#include "Display/RgbaPalette.h"
#include "Display/Tile.h"
#include "Display/Sprite.h"

class SpriteGenerator {
private:

	uint32_t manyTiles[32 * 8][32 * 8] = {};
	

public:
	std::vector<Sprite*> allSprites;
	//std::vector<Sprite> spritesPrioQueue[4];
	Tile objTileset[32][32];

	SpriteGenerator();
	void updateSpritePriorities();
	uint8_t* getTileset(bool is8bit);
	void update();
};