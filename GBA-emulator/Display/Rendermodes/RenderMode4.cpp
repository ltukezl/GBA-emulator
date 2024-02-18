#include "Gba-Graphics/Rendermodes/RenderMode4.h"
#include "Memory/MemoryOps.h"
#include "Gba-Graphics/Palette/RgbaPalette.h"

extern RgbaPalette PaletteColours;

RenderMode4::RenderMode4()
{
	background = (BGPixels*) new BGPixels;
	memset(background, 0, sizeof(BGPixels));
}

void RenderMode4::draw()
{
	uint32_t startAddr = 0xa000 * displayCtrl->displayFrame;
	for (size_t k = 0; k < 160; k++) {
		for (size_t i = 0; i < 240; i++) {
			uint8_t colorIdx = loadFromAddress(0x6000000 + startAddr++);
			auto paletteColor = PaletteColours.colorFromIndex(colorIdx);
			background->grid[k][i] = paletteColor;
		}
	}
}

void RenderMode4::fillImage(finalImagePalettes& imageBase)
{
	uint32_t startAddr = 0xa000 * displayCtrl->displayFrame;
	for (size_t k = 0; k < 160; k++) {
		for (size_t i = 0; i < 240; i++) {
			uint8_t colorIdx = loadFromAddress(0x6000000 + startAddr++);
			auto paletteColor = PaletteColours.colorFromIndex(colorIdx);
			if (imageBase[k][i].index != 0xFFFF && colorIdx == 0)
				continue;
			imageBase[k][i].palette = 0;
			imageBase[k][i].index = colorIdx;
		}
	}
}

uint32_t* RenderMode4::getBG()
{
	return (uint32_t*)&background->grid;
}
