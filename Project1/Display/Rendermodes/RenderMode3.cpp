#include "RenderMode3.h"
#include "Memory/MemoryOps.h"
#include "Display/RgbaPalette.h"

void RenderMode3::draw()
{
	uint32_t startAddr = 0;
	for (int k = 0; k < 160; k++){
		for (int i = 0; i < 240; i++) {
			ColorPaletteRam* colorPaletteRam = (ColorPaletteRam*)&VRAM[startAddr];
			int redScaled = colorPaletteRam->red * _scalar;
			int greenScaled = colorPaletteRam->green * _scalar;
			int blueScaled = colorPaletteRam->blue * _scalar;
			background->grid[k][i].r = redScaled;
			background->grid[k][i].b = blueScaled;
			background->grid[k][i].g = greenScaled;
			background->grid[k][i].a = 255;
			startAddr += 2;
		}
	}
}

RenderMode3::RenderMode3()
{
	background = (BGPixels*) new BGPixels;
	memset(background, 0, sizeof(BGPixels));
}

void RenderMode3::fillBg(uint32_t* imageBG)
{
	for (int k = 0; k < 160; k++) {
		for (int i = 0; i < 240; i++) {
			imageBG[512 * k + i] = background->grid[k][i].rawColor;
		}
	}
}

uint32_t* RenderMode3::getBG()
{
	return (uint32_t*)&background->grid;
}
