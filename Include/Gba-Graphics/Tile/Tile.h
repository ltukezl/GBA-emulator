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

    constexpr Tile(const uint32_t idx) : m_idx{idx * 32} {}
    const GBATile& create(const uint8_t paletteNum,
                          const bool flipV,
                          const bool flipH,
                          const bool is8bit);

private:
    alignas(32) GBATile m_tile = {};
    alignas(32) GBATile m_tileV = {};
    alignas(32) GBATile m_tileH = {};
    alignas(32) GBATile m_tileHV = {};

    const uint32_t m_idx;
    __m256i m_lastPalette;

    bool m_regular = false;
    bool m_fDone = false;
    bool m_hDone = false;
    bool m_fhDone = false;
};
