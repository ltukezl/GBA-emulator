#pragma once
#include <stdint.h>
#include <vector>
#include "Memory/memoryMappedIO.h"
#include "Display/Tile.h"
#include "Display/RgbaPalette.h"
#include <set>
#include <tuple>

extern class SpriteGenerator;

class Sprite
{
private:

	uint8_t _index;
	SpriteGenerator& _tileset;
	
	std::vector<Tile> tiles;

	void create1DSprite();
	void create2DSprite();

	Tile::BitmapBit* pixels;
	Tile::BitmapBit* tmpBuffer;

public:
	ObjReg1* objr1;
	ObjReg2* objr2;

	uint8_t sizeX;
	uint8_t sizeY;

	Sprite(SpriteGenerator& tileset, uint32_t address);
	void update();
	~Sprite();
	uint8_t* getSpriteTiles();
	void fillToImg(Tile::BitmapBit* imageBase);

	auto operator<(const Sprite& other) {	
		if (objr2->priority == other.objr2->priority)
			return _index < other._index;
		return objr2->priority < other.objr2->priority;
	}

	auto operator>(const Sprite& other) {
		if (objr2->priority == other.objr2->priority)
			return _index > other._index;
		return objr2->priority > other.objr2->priority;
	}

	auto operator==(const Sprite& other) {
		return objr2->priority == other.objr2->priority;
	}
};

