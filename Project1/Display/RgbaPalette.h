#pragma once

#include <stdint.h>
#include "Memory/memoryMappedIO.h"

class RgbaPalette {
private:
	const int _scalar = 255 / 31;
	static const int _colorsWidth = 16;
	static const int _colorsLength = 16;
	static const int _colorChannels = 4;
	union ColorPaletteRam* _colorStartAddress = nullptr;
	//NOTE: breaks for different endianess
	union GBAColor{
		struct {
			uint32_t r : 8;
			uint32_t g : 8;
			uint32_t b : 8;
			uint32_t a : 8;
		};
		uint32_t rawColor;
	};

	GBAColor paletteColorArray[_colorsWidth][_colorsLength] = { 0 };

public:
	bool palettesUpdated = true;

	RgbaPalette(ColorPaletteRam* startAddr);
	void updatePalette();
	uint8_t* getPalette();
	bool isInMemoryRange(uint8_t* testedAddress);

};