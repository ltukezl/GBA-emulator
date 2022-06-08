#pragma once
#include <stdint.h>
#include "Display/Tileset.h"

class TextMode
{

	Tile::GBATile backgroundTiles[64][64] = { 0 };
	uint8_t background[64 * 8][64 * 8] = { 0 };

	void draw(uint8_t regOffset);

};

