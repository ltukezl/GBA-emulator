#pragma once
#include <stdint.h>
#include "Display/Tileset.h"

class TextMode
{

	Tile::GBATile backgroundTiles[64][64] = { };
	uint32_t background[64 * 8][64 * 8] = { };
public:
	void draw(uint8_t regOffset);
	void fillImage(uint32_t* imageBase, uint32_t offset);
	uint32_t* getBG();
};

