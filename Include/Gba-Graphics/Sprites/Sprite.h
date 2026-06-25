#pragma once
#include <cstdint>

#include "Gba-Graphics/GBADrawable.hpp"
#include "Memory/memoryMappedIO.h"

using sprite_t = std::array<std::array<RgbaPalette::GBAColor, 64>, 64>;

struct SpriteTileset_t;

class BGLayer;

class Sprite
{
private:

    const auto& create1DSprite(SpriteTileset_t&);
    const auto& create2DSprite(SpriteTileset_t&);

    sprite_t m_pixels = {};

public:
    const sprite_t& create(SpriteTileset_t&);

    const uint32_t m_index;

    constexpr Sprite(const uint32_t idx) : m_index{idx} {}
    auto operator<=>(const Sprite& other) const { return false; }
    auto operator<=>(const BGLayer& other) const { return false; }
};
