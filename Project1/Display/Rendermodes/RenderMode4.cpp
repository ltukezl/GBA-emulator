#include "RenderMode4.h"
#include "Memory/MemoryOps.h"
#include "Display/RgbaPalette.h"

extern RgbaPalette PaletteColours;

RenderMode4::RenderMode4()
{
	background = (BGPixels*) new BGPixels;
	memset(background, 0, sizeof(BGPixels));
}

void RenderMode4::draw()
{
	uint32_t startAddr = 0xa000 * displayCtrl->displayFrame;
	for (int k = 0; k < 160; k++) {
		for (int i = 0; i < 240; i++) {
			uint8_t colorIdx = loadFromAddress(0x6000000 + startAddr++);
			auto paletteColor = PaletteColours.colorFromIndex(colorIdx);
			background->grid[k][i] = paletteColor;
		}
	}
}

uint32_t* RenderMode4::getBG()
{
	return (uint32_t*)&background->grid;
}
