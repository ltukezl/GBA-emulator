#include "Tile.h"
#include "Memory/MemoryOps.h"
#include "Memory/memoryMappedIO.h"

extern RgbaPalette PaletteColours;

Tile::Tile(uint32_t addr) {
	
	int startAddr = 0;

	for (int palette = 0; palette < 16; palette++) {
		for (int y = 0; y < 8; y++) {
			int row = rawLoad32(VRAM, addr + startAddr);
			for (int pixel = 0; pixel < 8; pixel++) {
				int color = (row & 0xf);
				auto paletteColor = PaletteColours.colorFromIndex(32 * palette + color);
				paletteColored[palette].grid[y][pixel] = paletteColor;
				row >>= 4;
			}
			startAddr += 4;
		}
	}
}

Tile::Tile(GBATile tile, bool is8Bit) {
	if (is8Bit) {

	}
	else {
		fourBitTile = tile;
	}
}


Tile Tile::flipVertical(bool is8Bit, bool flip) {
	if (!flip)
		return Tile(getTile(is8Bit, 0), is8Bit);
	GBATile tmpTile = getTile(is8Bit, 0);
	for (int i = 0; i < 8; i++) {
		for (int k = 0; k < 8; k++) {
			flippedTile.grid[i][k] = tmpTile.grid[i][7 - k];
		}
	}
	return Tile(flippedTile, is8Bit);
}

Tile Tile::flipHorizontal(bool is8Bit, bool flip){
	if (!flip)
		return Tile(getTile(is8Bit, 0), is8Bit);

	GBATile tmpTile = getTile(is8Bit, 0);
	for (int i = 0; i < 8; i++) {
		for (int k = 0; k < 8; k++) {
			flippedTile.grid[i][k] = tmpTile.grid[7 - i][k];
		}
	}
	return Tile(flippedTile, is8Bit);
}

Tile::GBATile Tile::getTile(bool is8Bit, uint8_t palette) {
	if (is8Bit)
		return eightBitTile;
	return paletteColored[palette];
}