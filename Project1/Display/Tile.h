#pragma once
#include <stdint.h>
#include <memory>
#include "Display/RgbaPalette.h"

class Tile
{
public:

	struct GBATile {
		union {
			RgbaPalette::GBAColor grid[8][8];
			uint8_t linear[8 * 8 * 4];
		};
		RgbaPalette::GBAColor transparent;

		GBATile& flipVertical(bool flip) {
			if (flip) {
				struct GBATile tmp = {};
				memcpy(tmp.linear, linear, sizeof(uint8_t) * 8 * 8 * 4);
				for (int i = 0; i < 8; i++) {
					for (int k = 0; k < 8; k++) {
						grid[i][k] = tmp.grid[7 - i][k];
					}
				}
			}
			return *this;
		}

		GBATile& flipHorizontal(bool flip) {
			if (flip) {
				struct GBATile tmp = {};
				memcpy(tmp.linear, linear, sizeof(uint8_t) * 8 * 8 * 4);
				for (int i = 0; i < 8; i++) {
					for (int k = 0; k < 8; k++) {
						grid[i][k] = tmp.grid[i][7 - k];
					}
				}
			}
			return *this;
		}
	};

	GBATile paletteColored[16] = {};
	Tile() = default;
	Tile(uint32_t addr, bool isObj);
	Tile(GBATile tile, bool is8Bit);
	GBATile& getTile(bool is8Bit, uint8_t palette);

private:

	RgbaPalette::GBAColor transparentColors[16] = {};

	GBATile fourBitTile = {};
	GBATile eightBitTile = {};
	GBATile flippedTile = {};

	uint32_t trasparentColor = 0;

};

