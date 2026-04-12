#pragma once
#include "Constants.h"
#include "Gba-Graphics/Palette/RgbaPalette.h"
#include "Gba-Graphics/Tile/Tile.h"
#include <cstdint>

class RenderMode4
{
private:

public:
    static void draw(finalImageColored& img, const uint32_t line);
};
