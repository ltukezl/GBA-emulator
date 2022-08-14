#include "Display/Tileset.h"
#include <iostream>


Tileset::Tileset() {}

void Tileset::update() {
	uint32_t startAddr = 0;
	for (int tileY = 0; tileY < 64; tileY++) {
		for (int tileX = 0; tileX < 32; tileX++) {
			Tile t(startAddr);
			tileset[tileY][tileX] = t;

			startAddr += 32;
		}
	}
}

uint8_t* Tileset::getTileset(bool is8bit) {
	for (int tileY = 0; tileY < 64; tileY++)
	for (int pixelY = 0; pixelY < 8; pixelY++)
	for (int tileX = 0; tileX < 32; tileX++)
	for (int pixelX = 0; pixelX < 8; pixelX++) {
		manyTiles[8 * tileY + pixelY][8 * tileX + pixelX] = tileset[tileY][tileX].getTile(is8bit, 10).grid[pixelY][pixelX].rawColor;
	}
	return (uint8_t*)manyTiles;
}

Tile::GBATile Tileset::getTile(uint8_t y, uint8_t x, uint8_t palette, bool is8Bit) {
	return tileset[y][x].getTile(is8Bit, palette);
}

Tile::GBATile Tileset::getTile(uint32_t index, uint8_t palette, bool is8Bit) {
	return tileset[index / 32][index % 32].getTile(is8Bit, palette);
}