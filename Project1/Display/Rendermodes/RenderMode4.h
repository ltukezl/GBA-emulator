#pragma once
#include <stdint.h>
#include "Display/Tile.h"
#include "Display/RgbaPalette.h"

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
