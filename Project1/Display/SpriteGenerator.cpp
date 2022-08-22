#include "Display/SpriteGenerator.h"

SpriteGenerator::SpriteGenerator()
{
}

void SpriteGenerator::update()
{
	uint32_t startAddr = 0x10000;
	for (int tileY = 0; tileY < 32; tileY++) {
		for (int tileX = 0; tileX < 32; tileX++) {
			Tile t(startAddr, true);
			objTileset[tileY][tileX] = t;

			startAddr += 32;
		}
	}
}

uint8_t* SpriteGenerator::getTileset(bool is8bit) {
	for (int tileY = 0; tileY < 32; tileY++)
		for (int pixelY = 0; pixelY < 8; pixelY++)
			for (int tileX = 0; tileX < 32; tileX++)
				for (int pixelX = 0; pixelX < 8; pixelX++) {
					manyTiles[8 * tileY + pixelY][8 * tileX + pixelX] = objTileset[tileY][tileX].getTile(is8bit, 0).grid[pixelY][pixelX].rawColor;
				}
	return (uint8_t*)manyTiles;
}