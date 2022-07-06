#include <stdint.h>
#include "RgbaPalette.h"
#include "Memory/memoryMappedIO.h"

RgbaPalette::RgbaPalette(ColorPaletteRam* startAddr): _colorStartAddress(startAddr) {

}

void RgbaPalette::updatePalette() {
	if (!palettesUpdated)
		return;

	int colorPtr = 0;

	//for bg palettes
	for (int i = 0; i < _colorsWidth; i++)
		for (int k = 0; k < _colorsLength; k++) {
			ColorPaletteRam* colorPaletteRam = &_colorStartAddress[colorPtr];
			int redScaled = colorPaletteRam->red * _scalar;
			int greenScaled = colorPaletteRam->green * _scalar;
			int blueScaled = colorPaletteRam->blue * _scalar;
			paletteColorArray[i][k].r = redScaled;
			paletteColorArray[i][k].g = greenScaled;
			paletteColorArray[i][k].b = blueScaled;
			paletteColorArray[i][k].a = 255;
			if(k == 0)
				paletteColorArray[i][k].a = 255;
			colorPtr++;
		}
}

RgbaPalette::GBAColor RgbaPalette::colorFromIndex(uint32_t index) {
	return paletteColorArray[index/16][index%16];
}
RgbaPalette::GBAColor RgbaPalette::colorFromIndex(uint32_t y, uint32_t x) {
	return paletteColorArray[y][x];
}

uint8_t* RgbaPalette::getPalette() {
	return (uint8_t*)paletteColorArray;
}

bool RgbaPalette::isInMemoryRange(uint8_t* testedAddress)
{
	return testedAddress >= (uint8_t*)_colorStartAddress && testedAddress < ((uint8_t*)_colorStartAddress + 1024);
}
