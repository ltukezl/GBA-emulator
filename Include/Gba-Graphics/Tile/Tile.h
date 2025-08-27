#pragma once
#include <array>
#include <cstdint>
#include <memory>

#include "Gba-Graphics/Palette/RgbaPalette.h"

class Tile
{
public:

    struct BitmapBit
    {
        uint16_t palette;
        uint16_t index;
    };
    struct GBATile
    {
        union
        {
            std::array<std::array<RgbaPalette::GBAColor, 8>, 8> grid;
            std::array<RgbaPalette::GBAColor, 8 * 8> linear;
        };
    };

    Tile() = default;
    const GBATile& create(const uint32_t addr,
                          const uint8_t paletteNum,
                          const bool flipV,
                          const bool flipH,
                          const bool is8bit);

private:
    GBATile m_tile = {};

    bool m_regular = false;
    bool m_fDone = false;
    bool m_hDone = false;
    bool m_fhDone = false;
};
