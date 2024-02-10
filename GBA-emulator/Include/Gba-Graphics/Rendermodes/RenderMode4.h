#pragma once
#include <cstdint>
#include "Gba-Graphics/Tile/Tile.h"
#include "Gba-Graphics/Palette/RgbaPalette.h"

class RenderMode4
{
private:
	struct BGPixels {
		RgbaPalette::GBAColor grid[512][512]; //consider of changing to 160x240
	};
	BGPixels* background = nullptr;


public:
	RenderMode4();
	void draw();
	uint32_t* getBG();
};
