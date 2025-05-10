#pragma once

#include <array>
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

	constexpr RgbaPalette(const union ColorPaletteRam* startAddr) : m_colorStartAddress(startAddr) {}
	void updatePalette();
	const uint8_t* getPalette();
	GBAColor colorFromIndex(uint32_t index) const;
	GBAColor colorFromIndex(uint32_t y, uint32_t x) const;

private:
	static constexpr uint32_t _scalar = 255 / 31;
	static constexpr uint8_t m_colorsWidth = 32;
	static constexpr uint8_t m_colorsLength = 16;
	static constexpr uint8_t _colorChannels = 4;
	const union ColorPaletteRam* m_colorStartAddress = nullptr;

	static constexpr uint32_t m_paletteStart = 0x500'0000;
	static constexpr uint32_t m_paletteEnd = 0x5FF'FFFF;
	
	union PaletteColorArray {
		std::array<GBAColor, m_colorsWidth * m_colorsLength> paletteColorArray_linear;
		std::array<std::array<GBAColor, m_colorsLength>, m_colorsWidth> paletteColorArray_2D;
	}paletteColorArray = {};
};