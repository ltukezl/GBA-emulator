#include "TextMode.h"
#include "Memory/MemoryOps.h"
#include "Memory/memoryMappedIO.h"
#include "Display/Tileset.h"
#include "Display/Rendermodes/TextMode.h"
#include <stdint.h>

extern Tileset tileset;

void TextMode::draw(uint8_t regOffset) {
	BgCnt* bgCnt = (BgCnt*)&IoRAM[8 + regOffset];
	uint32_t startAddr = bgCnt->bgBaseblock * 0x800;
	uint32_t tileBaseBlock = bgCnt->tileBaseBlock * 0x4000;

	for (int i = 0; i < 32; i++) {
		for (int k = 0; k < 32; k++) {
			BgTile* tileCtrl = (BgTile*)&VRAM[startAddr];
			backgroundTiles[i][k] = tileset.getTile(k, i, tileCtrl->paletteNum);
			startAddr += 2;
		}
	}

	if (bgCnt->hWide) {
		for (int i = 0; i < 32; i++) {
			for (int k = 0; k < 32; k++) {
				BgTile* tileCtrl = (BgTile*)&VRAM[startAddr];

				startAddr += 2;
			}
		}
	}
	if (bgCnt->vWide) {
		for (int i = 0; i < 32; i++) {
			for (int k = 0; k < 32; k++) {
				BgTile* tileCtrl = (BgTile*)&VRAM[startAddr];

				startAddr += 2;
			}
		}
	}
	if (bgCnt->vWide && bgCnt->hWide) {
		for (int i = 0; i < 32; i++) {
			for (int k = 0; k < 32; k++) {
				BgTile* tileCtrl = (BgTile*)&VRAM[startAddr];


				startAddr += 2;
			}
		}
	}
}