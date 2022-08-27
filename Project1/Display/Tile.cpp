#include "Tile.h"
#include "Memory/MemoryOps.h"
#include "Memory/memoryMappedIO.h"
#include <iostream>

extern RgbaPalette PaletteColours;

Tile::Tile(uint32_t addr, bool isObj) {
	
	int startAddr = 0;
	uint8_t offset = 0;
	uint8_t paletteBase4bit = isObj ? 16 : 0;
	uint8_t paletteBase8bit = isObj ? 256 : 0;
	
	for (int y = 0; y < 8; y++) {
		int row = rawLoad32(VRAM, addr + startAddr);
		for (int pixel = 0; pixel < 8; pixel++) {
			for (int palette = 0; palette < 16; palette++) {
				int color = (row & 0xf);
				auto paletteColor = PaletteColours.colorFromIndex(paletteBase4bit + palette, color);
				paletteColored[palette].grid[y][pixel] = paletteColor;	
			}
			row >>= 4;
		}
		startAddr += 4;
	}
	if (!isObj) {
		for (int y = 0; y < 8; y++) {
			for (int pixel = 0; pixel < 8; pixel++) {
				int color = rawLoad8(VRAM, addr * 2 + offset);
				auto paletteColor = PaletteColours.colorFromIndex(paletteBase8bit + color);
				eightBitTile.grid[y][pixel] = paletteColor;
				offset++;
			}
		}
	}

	for (int palette = 0; palette < 16; palette++) {
		transparentColors[palette] = PaletteColours.colorFromIndex(palette, 0);
	}
}

Tile::Tile(GBATile tile, bool is8Bit) {
	if (is8Bit) {
		fourBitTile = tile;
	}
	else {
		fourBitTile = tile;
	}
}

Tile::GBATile& Tile::getTile(bool is8Bit, uint8_t palette) {
	if (is8Bit)
		return eightBitTile;
	paletteColored[palette].transparent = transparentColors[palette];
	return paletteColored[palette];
}