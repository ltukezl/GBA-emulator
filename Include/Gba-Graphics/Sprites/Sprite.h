#pragma once
#include <cstdint>

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

    uint32_t get_priority() const;

    bool is_enabled() const;

    constexpr Sprite(const uint32_t idx) : m_index{idx} {}
    auto operator<=>(const Sprite& other) const
    {
        if (auto cmp = get_priority() <=> other.get_priority(); cmp != 0) {
            return cmp;
        }
        return m_index <=> other.m_index;
    }
    auto operator<=>(const BGLayer& other) const
    { return std::strong_ordering::less; }
};
