#pragma once
#include <cstdint>
#include "Gba-Graphics/Tile/Tile.h"
#include "Gba-Graphics/Palette/RgbaPalette.h"

class RenderMode3
{
private:
	static constexpr uint32_t _scalar = 255 / 31;
	struct BGPixels {
		RgbaPalette::GBAColor grid[512][512]; //consider of changing to 160x240
	};
	BGPixels* background = nullptr;


public:
	RenderMode3();
	void draw();
	uint32_t* getBG();
	void fillToDisplay(uint32_t* imageBG);
};
