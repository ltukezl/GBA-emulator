#pragma once
#include <cstdint>
#include "Constants.h"
#include "Gba-Graphics/Tile/Tile.h"
#include "Gba-Graphics/Palette/RgbaPalette.h"

class RenderMode5
{
private:
	static constexpr uint32_t _scalar = 255 / 31;
	struct BGPixels {
		RgbaPalette::GBAColor grid[512][512]; //consider of changing to 160x240
	};
	BGPixels* background = nullptr;


public:
	RenderMode5();
	void draw();
	uint32_t* getBG();
	void fillImage(finalImageColored& imageBase);
};
