#include "Sprite.h"
#include "Tile.h"
#include "Memory/memoryMappedIO.h"
#include "Memory/MemoryOps.h"
#include <iostream>

std::pair<uint8_t, uint8_t> shapes[3][4] =	{ { { 8, 8 }, { 16, 16 }, { 32, 32 }, { 64, 64 } },
											{ { 16, 8 }, { 32, 8 }, { 32, 16 }, { 64, 32 } },
											{ { 8, 16 }, { 8, 32 }, { 16, 32 }, { 32, 64 } } };

uint8_t* Sprite::getSpriteTiles() {
	uint16_t tilenum = 0;

	for (auto& tile : tiles) {
		for (uint8_t y = 0; y < sizeY; y++) {
			for (uint8_t x = 0; x < sizeX; x++) {
				auto t = tile.getTile(objr1->colorMode, objr2->paletteNumber);
				auto xPOS = ((tilenum % sizeX) * 8 + x);
				auto yPOS = 8 * 8 * sizeX * (tilenum / sizeX) + sizeX * y * 8;
				pixels[yPOS + xPOS] = t.grid[y][x].rawColor;
			}
		}
		tilenum++;
	}

	if (objr1->RotOrScale == 0) {
		if (objr1->parameter & (1 << 4)) { //horizontal flip
			memcpy(tmpBuffer, pixels, sizeof(uint32_t) * sizeX * 8 * sizeY * 8);
			for (uint16_t y = 0; y < sizeY * 8; y++) {
				for (uint16_t x = 0; x < sizeX * 8; x++) {
					pixels[sizeX * 8 * y + sizeX * 8 - x - 1] = tmpBuffer[sizeX * 8 * y + x];
				}
			}

		}
		if (objr1->parameter & (1 << 3)) { //vertical flip

			memcpy(tmpBuffer, pixels, sizeof(uint32_t) * sizeX * 8 * sizeY * 8);
			for (uint16_t y = 0; y < sizeY * 8; y++) {
				for (uint16_t x = 0; x < sizeX * 8; x++) {
					pixels[sizeX * 8 * y + x] = tmpBuffer[sizeX * 8 * (sizeY * 8 - y - 1) + x];
				}
			}

		}
	}
	return (uint8_t*)pixels;
}

void Sprite::create1DSprite() {
	uint16_t size = sizeX * sizeY;
	uint16_t startTile = objr2->tileNumber;
	for (uint16_t i = 0; i < size; i++) {
		tiles.push_back(_tileset.objTileset[(startTile % 1024) / 32][(startTile % 1024) % 32]);
		startTile++;
	}

}
void Sprite::create2DSprite() {
	uint16_t startTile = objr2->tileNumber;

	for (uint8_t y = 0; y < sizeY; y++) {
		uint16_t x_temp = startTile;
		for (uint8_t x = 0; x < sizeX; x++) {
			uint16_t yy = (x_temp / 32) % 32;
			uint16_t xx = (x_temp) % 32;
			tiles.push_back(_tileset.objTileset[yy][xx]);
			x_temp++;
		}
		startTile+=32;
	}

}


Sprite::Sprite(SpriteGenerator& tileset, uint32_t address): _tileset(tileset)
{
	objr1 = (ObjReg1*)&OAM[address + 0];
	objr2 = (ObjReg2*)&OAM[address + 4];
	auto dimensions = shapes[objr1->shape][objr1->shape];
	sizeX = dimensions.first;
	sizeY = dimensions.second;

	pixels    = (uint32_t*)new uint32_t[sizeX * 8 * sizeY * 8];
	tmpBuffer = (uint32_t*)new uint32_t[sizeX * 8 * sizeY * 8];

	if (displayCtrl->objectVRAMmap) {
		create1DSprite();
	}
	else {
		create2DSprite();
	}
}

Sprite::~Sprite() {
	delete pixels;
	delete tmpBuffer;
}
