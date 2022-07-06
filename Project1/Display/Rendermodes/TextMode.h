#pragma once
#include <stdint.h>
#include "Display/Tileset.h"

class TextMode
{

	Tile::GBATile backgroundTiles[64][64] = { 0 };
	uint32_t background[64 * 8][64 * 8] = { 0 };
public:
	void draw(uint8_t regOffset);
	uint32_t* getBG();
};

