#pragma once

#include "Gba-Graphics/Tile/Tile.h"
#include <array>
#include <utility>

class Tileset
{
public:

    union Tileset_t
    {
        std::array<std::array<Tile, 32>, 32> grid;
        std::array<Tile, 32 * 32> linear;

        template<std::size_t... I>
        constexpr Tileset_t(std::index_sequence<I...>) :
            linear{Tile(static_cast<uint32_t>(I))...}
        {
        }
    };

    Tileset_t tileset{std::make_index_sequence<32 * 32>{}};

    void update();
    uint8_t* getTileset(const bool is8bit);
    Tile::GBATile getTile(const uint8_t y,
                          const uint8_t x,
                          const uint8_t palette,
                          const bool is8Bit);
    Tile::GBATile
    getTile(const uint32_t index, const uint8_t palette, const bool is8Bit);
private:

    // uint32_t fullTileset[64 * 8][32 * 8] = {};
};
