#pragma once

#include <vector>
#include "Gba-Graphics/Palette/RgbaPalette.h"
#include "Gba-Graphics/Tile/Tile.h"
#include "Gba-Graphics/Sprites/Sprite.h"

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
