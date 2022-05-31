#pragma once

#include <stdint.h>
#include "Memory/memoryMappedIO.h"

class RgbaPalette {

public:
	//NOTE: breaks for different endianess
	union GBAColor {
		struct {
			uint32_t r : 8;
			uint32_t g : 8;
			uint32_t b : 8;
			uint32_t a : 8;
		};
		uint32_t rawColor;
	};

	bool palettesUpdated = true;

	RgbaPalette(union ColorPaletteRam* startAddr);
	void updatePalette();
	uint8_t* getPalette();
	GBAColor colorFromIndex(uint32_t index);
	GBAColor colorFromIndex(uint32_t y, uint32_t x);
	bool isInMemoryRange(uint8_t* testedAddress);

private:
	const int _scalar = 255 / 31;
	static const int _colorsWidth = 32;
	static const int _colorsLength = 16;
	static const int _colorChannels = 4;
	union ColorPaletteRam* _colorStartAddress = nullptr;
	
	GBAColor paletteColorArray[_colorsWidth][_colorsLength] = { 0 };

};