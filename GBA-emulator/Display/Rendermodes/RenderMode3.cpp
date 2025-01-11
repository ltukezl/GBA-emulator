#include "Gba-Graphics/Rendermodes/RenderMode3.h"
#include "Gba-Graphics/Palette/RgbaPalette.h"
#include "Memory/MemoryOps.h"

void RenderMode3::draw()
{
	uint32_t startAddr = 0;
	for (size_t k = 0; k < 160; k++){
		for (size_t i = 0; i < 240; i++) {
			ColorPaletteRam* colorPaletteRam = (ColorPaletteRam*)&vram[startAddr];
			uint32_t redScaled = colorPaletteRam->red * _scalar;
			uint32_t greenScaled = colorPaletteRam->green * _scalar;
			uint32_t blueScaled = colorPaletteRam->blue * _scalar;
			background->grid[k][i].r = redScaled;
			background->grid[k][i].b = blueScaled;
			background->grid[k][i].g = greenScaled;
			startAddr += 2;
		}
	}
}

RenderMode3::RenderMode3()
{
	background = (BGPixels*) new BGPixels;
	memset(background, 0xFFFF'FFFF, sizeof(BGPixels));
}

void RenderMode3::fillToDisplay(finalImageColored& imageBG)
{
	for (int k = 0; k < 160; k++) {
		memcpy(&imageBG[k], &background->grid[k], 240*4);
	}
}

uint32_t* RenderMode3::getBG()
{
	return (uint32_t*)&background->grid;
}
