#include "Sprite.h"
#include "Tile.h"
#include "Memory/memoryMappedIO.h"
#include "Memory/MemoryOps.h"
#include "Display/SpriteGenerator.h"
#include <iostream>

std::pair<uint8_t, uint8_t> shapes[3][4] =	{ { { 8, 8 }, { 16, 16 }, { 32, 32 }, { 64, 64 } },
											{ { 16, 8 }, { 32, 8 }, { 32, 16 }, { 64, 32 } },
											{ { 8, 16 }, { 8, 32 }, { 16, 32 }, { 32, 64 } } };

uint8_t* Sprite::getSpriteTiles() {
	uint16_t tilenum = 0;
	uint8_t tst = 0;
	
	for (auto& tile : tiles) {
		auto t = tile.getTile(objr1->colorMode, 16+objr2->paletteNumber);
		for (uint8_t y = 0; y < 8; y++) {
			auto yPOS = (tilenum /(sizeX / 8)) * 8 * 8 * (sizeX / 8) + y * sizeX;
			for (uint8_t x = 0; x < 8; x++) {
				auto xPOS = ((8 * tilenum) % sizeX + x);
				auto tt = t.grid[y][x];
				pixels[yPOS + xPOS] = tt;
			}
		}
		tilenum++;
	}
	
	
	if (objr1->RotOrScale == 0) {
		if (objr1->parameter & (1 << 3)) { //horizontal flip
			memcpy(tmpBuffer, pixels, sizeof(uint8_t) * sizeX * 8 * sizeY * 8);
			for (uint16_t y = 0; y < sizeY; y++) {
				for (uint16_t x = 0; x < sizeX; x++) {
					pixels[sizeX * y + sizeX - x - 1] = tmpBuffer[sizeX * y + x];
				}
			}

		}
		if (objr1->parameter & (1 << 4)) { //vertical flip

			memcpy(tmpBuffer, pixels, sizeof(uint8_t) * sizeX * 8 * sizeY * 8);
			for (uint16_t y = 0; y < sizeY; y++) {
				for (uint16_t x = 0; x < sizeX; x++) {
					pixels[sizeX * y + x] = tmpBuffer[sizeX * (sizeY - y - 1) + x];
				}
			}

		}
	}
	
	return (uint8_t*)pixels;
}

void Sprite::fillToImg(Tile::BitmapBit* imageBase) {
	if (objr1->isDoubleOrNoDisplay)
		return;
	getSpriteTiles();
	auto asd = 0;
	for (int k = 0; k < sizeY; k++) {
		for (int i = 0; i < sizeX; i++) {
			if ((objr1->xCoord + i) >= 240 || (objr1->xCoord + i) < 0) {
				asd++;
				continue;
			}
			if ((objr1->yCoord + k) >= 160) {
				asd++;
				continue;
			}

			auto xpos = (objr1->xCoord + i);
			auto ypos = 240 * (objr1->yCoord + k);
			auto paletteInfo = pixels[asd++];
			if (paletteInfo.index == 0)
				continue;
			imageBase[ypos + xpos] = paletteInfo;
		}
	}
}

void Sprite::create1DSprite() {
	uint16_t size = (sizeX / 8) * (sizeY / 8);
	uint16_t startTile = objr2->tileNumber;
	for (uint16_t i = 0; i < size; i++) {
		tiles.push_back(_tileset.objTileset[(startTile % 1024) / 32][(startTile % 1024) % 32]);
		startTile++;
	}
}

void Sprite::create2DSprite() {
	uint16_t startTile = objr2->tileNumber;

	for (uint8_t y = 0; y < sizeY / 8; y++) {
		uint16_t x_temp = startTile;
		for (uint8_t x = 0; x < sizeX / 8 ; x++) {
			uint16_t yy = (x_temp / 32) % 32;
			uint16_t xx = (x_temp) % 32;
			tiles.push_back(_tileset.objTileset[yy][xx]);
			x_temp++;
		}
		startTile += 32;
	}
}

void Sprite::update() {
	tiles.clear();
	auto dimensions = shapes[objr1->shape][objr1->size];
	sizeX = dimensions.first;
	sizeY = dimensions.second;

	if (displayCtrl->objectVRAMmap) {
		create1DSprite();
	}
	else {
		create2DSprite();
	}
}


Sprite::Sprite(SpriteGenerator& tileset, uint32_t address): _tileset(tileset), _index(address/8)
{
	objr1 = (ObjReg1*)&OAM[address + 0];
	objr2 = (ObjReg2*)&OAM[address + 4];
	auto dimensions = shapes[objr1->shape][objr1->size];
	sizeX = dimensions.first;
	sizeY = dimensions.second;

	pixels    = (Tile::BitmapBit*)new Tile::BitmapBit[8*8*32 * 32];
	tmpBuffer = (Tile::BitmapBit*)new Tile::BitmapBit[8*8*32 * 32];

	memset(pixels, 0, sizeof(Tile::BitmapBit) * 8*8*32 * 32);
}

Sprite::~Sprite() {
	delete pixels;
	delete tmpBuffer;
}
