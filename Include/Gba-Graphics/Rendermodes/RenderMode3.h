#pragma once
#include "Constants.h"
#include "Gba-Graphics/Palette/RgbaPalette.h"
#include "Gba-Graphics/Tile/Tile.h"
#include <cstdint>

class RenderMode3
{
private:
    static constexpr uint32_t _scalar = 255 / 31;
    finalImageColored& m_finalBg;

public:
    RenderMode3(finalImageColored& imageBG) : m_finalBg{imageBG} {}
    static void draw(finalImageColored& img, const uint32_t line);
};
