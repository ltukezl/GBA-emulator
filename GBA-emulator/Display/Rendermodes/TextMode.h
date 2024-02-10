#pragma once
#include <stdint.h>
#include "Display/Tileset.h"
#include "Display/Tile.h"

class TextMode
{

	Tile::GBATile backgroundTiles[64][64] = { };
	Tile::BitmapBit background[64 * 8][64 * 8] = { };
	uint32_t backgroundColored[64 * 8][64 * 8] = { };
public:
	void draw(uint8_t regOffset);
	void fillImage(Tile::BitmapBit* imageBase, uint32_t offset);
	uint32_t* getBG();
};

