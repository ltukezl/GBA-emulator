#include "TextMode.h"
#include "Memory/MemoryOps.h"
#include "Memory/memoryMappedIO.h"
#include "Display/Tileset.h"
#include "Display/Rendermodes/TextMode.h"
#include <stdint.h>
#include <iostream>

extern Tileset tileset;

void TextMode::draw(uint8_t regOffset) {
	BgCnt* bgCnt = (BgCnt*)&IoRAM[8 + regOffset];
	uint32_t startAddr = bgCnt->bgBaseblock * 0x800;
	//uint32_t tileBaseBlock = bgCnt->tileBaseBlock * 0x4000;
	uint32_t tileStartRow = bgCnt->is8Bit ? bgCnt->tileBaseBlock * 8 : bgCnt->tileBaseBlock * 16;
	uint8_t sizeX = bgCnt->hWide ? 64 : 32;
	uint8_t sizeY = bgCnt->vWide ? 64 : 32;

	for (int i = 0; i < 32; i++) {
		for (int k = 0; k < 32; k++) {
			BgTile* tileCtrl = (BgTile*)&VRAM[startAddr];
			auto t = tileset.getTile(tileStartRow + tileCtrl->tileNumber / 32, tileCtrl->tileNumber % 32, tileCtrl->paletteNum, bgCnt->is8Bit);
			backgroundTiles[i][k] = t;
			startAddr += 2;
		}
	}

	if (bgCnt->hWide) {
		for (int i = 0; i < 32; i++) {
			for (int k = 0; k < 32; k++) {
				BgTile* tileCtrl = (BgTile*)&VRAM[startAddr];
				backgroundTiles[i][k+32] = tileset.getTile(tileStartRow + tileCtrl->tileNumber / 32, tileCtrl->tileNumber % 32, tileCtrl->paletteNum, bgCnt->is8Bit);
				startAddr += 2;
			}
		}
	}
	if (bgCnt->vWide) {
		for (int i = 0; i < 32; i++) {
			for (int k = 0; k < 32; k++) {
				BgTile* tileCtrl = (BgTile*)&VRAM[startAddr];
				backgroundTiles[i+32][k] = tileset.getTile(tileStartRow + tileCtrl->tileNumber / 32, tileCtrl->tileNumber % 32, tileCtrl->paletteNum, bgCnt->is8Bit);
				startAddr += 2;
			}
		}
	}
	if (bgCnt->vWide && bgCnt->hWide) {
		for (int i = 0; i < 32; i++) {
			for (int k = 0; k < 32; k++) {
				BgTile* tileCtrl = (BgTile*)&VRAM[startAddr];
				backgroundTiles[i+32][k+32] = tileset.getTile(tileStartRow + tileCtrl->tileNumber / 32, tileCtrl->tileNumber % 32, tileCtrl->paletteNum, bgCnt->is8Bit);
				startAddr += 2;
			}
		}
	}

	for (int tileY = 0; tileY < sizeY; tileY++)
	for (int pixelY = 0; pixelY < 8; pixelY++)
	for (int tileX = 0; tileX < sizeX; tileX++)
	for (int pixelX = 0; pixelX < 8; pixelX++) {
		background[8 * tileY + pixelY][8 * tileX + pixelX] = backgroundTiles[tileY][tileX].grid[pixelY][pixelX].rawColor;
	}
}

uint32_t* TextMode::getBG() {
	return (uint32_t*)background;
}