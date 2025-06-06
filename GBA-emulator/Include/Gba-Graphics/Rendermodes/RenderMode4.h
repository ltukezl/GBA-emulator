#pragma once
#include <cstdint>
#include "Constants.h"
#include "Gba-Graphics/Tile/Tile.h"
#include "Gba-Graphics/Palette/RgbaPalette.h"

class RenderMode4
{
private:
	struct BGPixels {
		RgbaPalette::GBAColor grid[512][512]; //consider of changing to 160x240
	} m_background;

public:
	void draw();
	uint32_t* getBG();
	void fillImage(finalImagePalettes& imageBase);
};
