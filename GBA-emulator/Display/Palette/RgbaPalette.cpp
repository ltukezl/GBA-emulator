#include <cstdint>
#include "Gba-Graphics/Palette/RgbaPalette.h"
#include "Memory/memoryMappedIO.h"
#include "Display/Display.h"

void RgbaPalette::updatePalette() {
	if (!palettesUpdated)
		return;

	size_t colorPtr = 0;

	//for bg palettes
	for (size_t i = 0; i < m_colorsWidth; i++)
		for (size_t k = 0; k < m_colorsLength; k++) {
			const ColorPaletteRam* colorPaletteRam = &m_colorStartAddress[colorPtr];
			uint32_t redScaled = colorPaletteRam->red * _scalar;
			uint32_t greenScaled = colorPaletteRam->green * _scalar;
			uint32_t blueScaled = colorPaletteRam->blue * _scalar;
			paletteColorArray.paletteColorArray_2D[i][k].r = redScaled;
			paletteColorArray.paletteColorArray_2D[i][k].g = greenScaled;
			paletteColorArray.paletteColorArray_2D[i][k].b = blueScaled;
			paletteColorArray.paletteColorArray_2D[i][k].a = 255;
			colorPtr++;
		}

	palettesUpdated = false;
	debugView->VRAMupdated = true;
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

void RgbaPalette::paletteMemChanged(uint32_t testedAddress)
{
	if (testedAddress >= m_paletteStart && testedAddress <= m_paletteEnd)
		palettesUpdated = true;
}
