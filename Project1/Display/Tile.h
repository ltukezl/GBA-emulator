#pragma once
#include <stdint.h>
#include "Display/RgbaPalette.h"

class Tile
{
public:

	union GBATile {
		RgbaPalette::GBAColor grid[8][8];
		uint8_t linear[8 * 8 * 4];
	};

	GBATile paletteColored[16] = {};

	Tile() = default;
	Tile(uint32_t addr);
	Tile(GBATile tile, bool is8Bit);
	Tile flipVertical(bool is8Bit, bool flip);
	Tile flipHorizontal(bool is8Bit, bool flip);
	GBATile& getTile(bool is8Bit, uint8_t palette);

private:

	RgbaPalette::GBAColor transparentColors[16] = {};

	GBATile fourBitTile = { 0 };
	GBATile eightBitTile = { 0 };
	GBATile flippedTile = { 0 };

	uint32_t trasparentColor = 0;

};

