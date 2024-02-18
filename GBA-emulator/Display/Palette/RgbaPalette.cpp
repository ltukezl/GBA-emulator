#include <cstdint>
#include "Gba-Graphics/Palette/RgbaPalette.h"
#include "Memory/memoryMappedIO.h"
#include "Display/Display.h"
#include "Memory/memoryOps.h"

void RgbaPalette::updatePalette() {
	if (!paletteram.m_accessed)
		return;

	for (size_t i = 0; i < paletteram.m_accessedPaletteColour.size(); i++)
	{
		if (!paletteram.m_accessedPaletteColour[i])
			continue;
		const ColorPaletteRam* colorPaletteRam = &m_colorStartAddress[i];
		uint32_t redScaled = colorPaletteRam->red * _scalar;
		uint32_t greenScaled = colorPaletteRam->green * _scalar;
		uint32_t blueScaled = colorPaletteRam->blue * _scalar;
		paletteColorArray.paletteColorArray_linear[i].r = redScaled;
		paletteColorArray.paletteColorArray_linear[i].g = greenScaled;
		paletteColorArray.paletteColorArray_linear[i].b = blueScaled;
		paletteColorArray.paletteColorArray_linear[i].a = 255;
		paletteram.m_accessedPaletteColour[i] = 0;
	}

	debugView->VRAMupdated = true;
	paletteram.clearAccess();
}

RgbaPalette::GBAColor RgbaPalette::colorFromIndex(uint32_t index) const {
	return paletteColorArray.paletteColorArray_linear[index];
}
RgbaPalette::GBAColor RgbaPalette::colorFromIndex(uint32_t y, uint32_t x) const {
	return paletteColorArray.paletteColorArray_2D[y][x];
}

const uint8_t* RgbaPalette::getPalette() {
	return reinterpret_cast<uint8_t*>(paletteColorArray.paletteColorArray_linear.data());
}
