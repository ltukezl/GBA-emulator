#include "Gba-Graphics/Rendermodes/RenderMode5.h"
#include "Memory/MemoryOps.h"
#include "Gba-Graphics/Palette/RgbaPalette.h"

extern RgbaPalette PaletteColours;

RenderMode5::RenderMode5()
{
	background = (BGPixels*) new BGPixels;
	memset(background, 0xFFFF'FFFF, sizeof(BGPixels));
}

uint32_t* RenderMode5::getBG()
{
	return (uint32_t*)&background->grid;
}


void RenderMode5::draw()
{
	uint32_t startAddr = 0xa000 * displayCtrl->displayFrame;
	for (size_t k = 0; k < 128; k++) {
		for (size_t i = 0; i < 160; i++) {
			ColorPaletteRam* colorPaletteRam = (ColorPaletteRam*)&vram[startAddr];
			uint32_t redScaled = colorPaletteRam->red * _scalar;
			uint32_t greenScaled = colorPaletteRam->green * _scalar;
			uint32_t blueScaled = colorPaletteRam->blue * _scalar;
			background->grid[k][i].r = redScaled;
			background->grid[k][i].b = blueScaled;
			background->grid[k][i].g = greenScaled;
			startAddr += 2;
		}
		for (size_t i = 160; i < 240; i++) {
			background->grid[k][i] = PaletteColours.colorFromIndex(0);
		}
	}

	for (size_t k = 128; k < 160; k++) {
		for (size_t i = 0; i < 240; i++) {
			background->grid[k][i] = PaletteColours.colorFromIndex(0);
		}
	}
}

void RenderMode5::fillImage(finalImageColored& imageBase)
{
	for (int k = 0; k < 160; k++) {
		memcpy(&imageBase[k], &background->grid[k], 240*4);
	}
}