#include <iostream>
#include <algorithm>

#include "Gba-Graphics/Sprites/SpriteGenerator.h"

extern RgbaPalette PaletteColours;

SpriteGenerator::SpriteGenerator()
{
	allSprites.reserve(127);
	for (uint8_t i = 0; i < 127; i++) {
		auto s = new Sprite(*this, i * 8);
		allSprites.push_back(s);
	}
}

void SpriteGenerator::updateSpritePriorities()
{
	std::sort(std::begin(allSprites), std::end(allSprites));
}

void SpriteGenerator::update()
{
	//for (auto& vec : spritesPrioQueue) {
		//vec.clear();
	//}

	uint32_t startAddr = 0x10000;
	for (int tileY = 0; tileY < 32; tileY++) {
		for (int tileX = 0; tileX < 32; tileX++) {
			Tile t(startAddr, true);
			objTileset[tileY][tileX] = t;

			startAddr += 32;
		}
	}
	
	for (auto& sprite : allSprites) {
		sprite->update();
	}	
}

uint8_t* SpriteGenerator::getTileset(bool is8bit) {
	for (int tileY = 0; tileY < 32; tileY++)
		for (int pixelY = 0; pixelY < 8; pixelY++)
			for (int tileX = 0; tileX < 32; tileX++)
				for (int pixelX = 0; pixelX < 8; pixelX++) {
					auto& t = objTileset[tileY][tileX].getTile(is8bit, 16).grid[pixelY][pixelX];
					auto clr = PaletteColours.colorFromIndex(t.palette, t.index);
					manyTiles[8 * tileY + pixelY][8 * tileX + pixelX] = clr.rawColor;
				}
	return (uint8_t*)manyTiles;
}