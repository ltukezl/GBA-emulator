#include "Display/Tileset.h"
#include "Display/RgbaPalette.h"
#include <iostream>

extern RgbaPalette PaletteColours;

Tileset::Tileset() {}

void Tileset::update() {
	uint32_t startAddr = 0;
	for (int tileY = 0; tileY < 64; tileY++) {
		for (int tileX = 0; tileX < 32; tileX++) {
			Tile t(startAddr, false);
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
		auto tile = tileset[tileY][tileX].getTile(is8bit, 0).grid[pixelY][pixelX];
		auto clr = PaletteColours.colorFromIndex(tile.palette, tile.index);
		fullTileset[8 * tileY + pixelY][8 * tileX + pixelX] = clr.rawColor;
	}
	return (uint8_t*)fullTileset;
}

Tile::GBATile Tileset::getTile(uint8_t y, uint8_t x, uint8_t palette, bool is8Bit) {
	return tileset[y][x].getTile(is8Bit, palette);
}

Tile::GBATile Tileset::getTile(uint32_t index, uint8_t palette, bool is8Bit) {
	return tileset[index / 32][index % 32].getTile(is8Bit, palette);
}