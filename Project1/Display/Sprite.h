#pragma once
#include <stdint.h>
#include <vector>
#include "Memory/memoryMappedIO.h"
#include "Display/SpriteGenerator.h"

class Sprite
{
private:

	SpriteGenerator& _tileset;
	
	std::vector<Tile> tiles;

	void create1DSprite();
	void create2DSprite();

	uint32_t* pixels;
	uint32_t* tmpBuffer;

public:
	ObjReg1* objr1;
	ObjReg2* objr2;

	uint8_t sizeX;
	uint8_t sizeY;

	Sprite(SpriteGenerator& tileset, uint32_t address);
	~Sprite();
	uint8_t* getSpriteTiles();
};

