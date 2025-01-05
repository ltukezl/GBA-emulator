#include "Gba-Graphics/Rendermodes/RenderMode4.h"
#include "Memory/MemoryOps.h"
#include "Gba-Graphics/Palette/RgbaPalette.h"

extern RgbaPalette PaletteColours;

void RenderMode4::draw()
{
	uint32_t startAddr = 0xa000 * displayCtrl->displayFrame;
	for (size_t k = 0; k < 160; k++) {
		for (size_t i = 0; i < 240; i++) {
			uint8_t colorIdx = vram.getMemoryPtr()[startAddr++];
			auto paletteColor = PaletteColours.colorFromIndex(colorIdx);
			m_background.grid[k][i] = paletteColor;
		}
	}
}

void RenderMode4::fillImage(finalImagePalettes& imageBase)
{
	uint32_t startAddr = 0xa000 * displayCtrl->displayFrame;
	for (size_t k = 0; k < 160; k++) {
		for (size_t i = 0; i < 240; i++) {
			uint8_t colorIdx = vram.getMemoryPtr()[startAddr++];
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
	return static_cast<uint32_t*>(&m_background.grid[0][0].rawColor);
}
