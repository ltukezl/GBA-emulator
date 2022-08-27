#pragma once
#include <stdint.h>
#include "Display/Tile.h"
#include "Display/RgbaPalette.h"

class RenderMode3
{
private:
	const int _scalar = 255 / 31;
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
