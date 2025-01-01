#include "Gba-Graphics/Tile/Tileset.h"
#include "Gba-Graphics/Palette/RgbaPalette.h"
#include "Memory/MemoryOps.h"
#include <iostream>

extern RgbaPalette PaletteColours;

void Tileset::update() {
	uint32_t startAddr = 0;
	for (size_t tileY = 0; tileY < 64; tileY++) {
		for (size_t tileX = 0; tileX < 32; tileX++) {
			Tile t(startAddr, false);
			tileset.grid[tileY][tileX] = std::move(t);

			startAddr += 32;
			if (startAddr == 0xdd00)
			{
				continue;
			}
		}
	}
}

uint8_t* Tileset::getTileset(bool is8bit) {
	for (size_t tileY = 0; tileY < 64; tileY++)
	for (size_t pixelY = 0; pixelY < 8; pixelY++)
	for (size_t tileX = 0; tileX < 32; tileX++)
	for (size_t pixelX = 0; pixelX < 8; pixelX++) {
		auto& tile = tileset.grid[tileY][tileX].getTile(is8bit, 0).grid[pixelY][pixelX];
		auto clr = PaletteColours.colorFromIndex(tile.palette, tile.index);
		fullTileset[8 * tileY + pixelY][8 * tileX + pixelX] = clr.rawColor;
	}
	return (uint8_t*)fullTileset;
}

Tile::GBATile Tileset::getTile(const uint8_t y, const uint8_t x, const uint8_t palette, const bool is8Bit) {
	return tileset.grid[y][x].getTile(is8Bit, palette);
}

Tile::GBATile Tileset::getTile(const uint32_t index, const uint8_t palette, const bool is8Bit) {
	return tileset.linear[index].getTile(is8Bit, palette);
}