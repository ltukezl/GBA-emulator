#include "RenderMode5.h"
#include "Memory/MemoryOps.h"
#include "Display/RgbaPalette.h"

RenderMode5::RenderMode5()
{
	background = (BGPixels*) new BGPixels;
	memset(background, 0, sizeof(BGPixels));
}

uint32_t* RenderMode5::getBG()
{
	return (uint32_t*)&background->grid;
}


void RenderMode5::draw()
{
	uint32_t startAddr = 0xa000 * displayCtrl->displayFrame;
	for (int k = 0; k < 128; k++) {
		for (int i = 0; i < 160; i++) {
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

