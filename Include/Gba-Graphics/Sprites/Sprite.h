#pragma once
#include <cstdint>
#include <vector>
#include <set>
#include <tuple>

#include "Constants.h"
#include "Memory/memoryMappedIO.h"
#include "Gba-Graphics/Tile/Tile.h"
#include "Gba-Graphics/Palette/RgbaPalette.h"

class SpriteGenerator;

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
	void fillToImg(const RgbaPalette& palette, finalImagePalettes& imageBase, const bool tst);

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

