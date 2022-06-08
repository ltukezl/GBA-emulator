#include "Display/Tileset.h"


Tileset::Tileset() {}

void Tileset::update() {
	uint32_t startAddr = 0;
	for (int palette = 0; palette < 16; palette++) {
		for (int tileY = 0; tileY < 64; tileY++) {
			for (int tileX = 0; tileX < 32; tileX++) {
				Tile t(startAddr);
				tileset[palette][tileY][tileX] = t.getTile(false, palette);
				startAddr += 32;
			}
		}
	}
}

uint8_t* Tileset::getTileset(bool is8bit) {
	for (int tileY = 0; tileY < 64; tileY++)
	for (int pixelY = 0; pixelY < 8; pixelY++)
	for (int tileX = 0; tileX < 32; tileX++)
	for (int pixelX = 0; pixelX < 8; pixelX++) {
		manyTiles[8 * tileY + pixelY][8 * tileX + pixelX] = tileset[0][tileY][tileX].grid[pixelY][pixelX].rawColor;
	}
	return (uint8_t*)manyTiles;
}

Tile::GBATile Tileset::getTile(uint8_t x, uint8_t y, uint8_t palette) {
	return tileset[palette][x][y];
}