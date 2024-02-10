#pragma once

#include <cstdint>
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

		bool operator==(GBAColor& other) { return rawColor == other.rawColor; }
	};

	bool palettesUpdated = true;

	RgbaPalette(union ColorPaletteRam* startAddr);
	void updatePalette();
	uint8_t* getPalette();
	GBAColor colorFromIndex(uint32_t index);
	GBAColor colorFromIndex(uint32_t y, uint32_t x);
	void paletteMemChanged(uint32_t testedAddress);

private:
	const int _scalar = 255 / 31;
	static constexpr int _colorsWidth = 32;
	static constexpr int _colorsLength = 16;
	static constexpr int _colorChannels = 4;
	union ColorPaletteRam* _colorStartAddress = nullptr;

	const uint32_t _paletteStart = 0x5000000;
	const uint32_t _paletteEnd = 0x5FFFFFF;
	
	GBAColor paletteColorArray[_colorsWidth][_colorsLength] = {};
	
};