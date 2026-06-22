#pragma once

#include "Gba-Graphics/Sprites/Sprite.h"
#include "Gba-Graphics/Tile/Tile.h"
#include <mdspan>

struct SpriteTileset_t
{
    std::array<Tile, 32 * 32> linear;
    std::mdspan<Tile, std::extents<std::size_t, 4, 256>> grid{linear.data(), 4,
                                                              256};

    template<std::size_t... I>
    constexpr SpriteTileset_t(std::index_sequence<I...>) :
        linear{Tile(static_cast<uint32_t>(I) + 2048)...}
    {
    }
};

class SpriteSet
{
private:

    struct Spriteset_t
    {
        std::array<Sprite, 128> linear;

        template<std::size_t... I>
        constexpr Spriteset_t(std::index_sequence<I...>) :
            linear{Sprite(static_cast<uint32_t>(I))...}
        {
        }
    };


public:
    SpriteTileset_t m_tileset{std::make_index_sequence<32 * 32>{}};
    Spriteset_t m_sprite_set{std::make_index_sequence<128>{}};
};
