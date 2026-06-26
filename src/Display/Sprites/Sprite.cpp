#include "Gba-Graphics/Sprites/Sprite.h"
#include "Gba-Graphics/Sprites/SpriteSet.h"
#include "Gba-Graphics/Tile/Tile.h"
#include "Memory/memoryMappedIO.h"
#include "Memory/memoryOps.h"

extern RgbaPalette PaletteColours;

// constexpr std::pair<uint8_t, uint8_t> shapes[3][4] = {
//     {{8, 8}, {16, 16}, {32, 32}, {64, 64}},
//     {{16, 8}, {32, 8}, {32, 16}, {64, 32}},
//     {{8, 16}, {8, 32}, {16, 32}, {32, 64}}};

constexpr std::pair<uint8_t, uint8_t> shapes[3][4] = {
    {{1, 1}, {2, 2}, {4, 4}, {8, 8}},
    {{2, 1}, {4, 1}, {4, 2}, {8, 4}},
    {{1, 2}, {1, 4}, {2, 4}, {4, 8}}};

const auto& Sprite::create1DSprite(SpriteTileset_t& sprite_tiles)
{
    const ObjReg1* objr1 =
        reinterpret_cast<ObjReg1*>(oamRam.getMemoryPtr() + m_index * 8);
    const ObjReg2* objr2 =
        reinterpret_cast<ObjReg2*>(oamRam.getMemoryPtr() + m_index * 8 + 4);
    const auto dimensions = shapes[objr1->shape][objr1->size];

    const uint32_t tile_start = objr2->tileNumber;

    uint32_t i = 0;
    for (size_t major_y = 0; major_y < dimensions.second; major_y++) {
        for (size_t major_x = 0; major_x < dimensions.first; major_x++) {
            const auto& tile = sprite_tiles.linear[tile_start + i].create(
                16 + objr2->paletteNumber, false, false, objr1->colorMode);

            for (size_t x = 0; x < 8; x++) {
                for (size_t y = 0; y < 8; y++) {
                    m_pixels[major_y * 8 + y][major_x * 8 + x] =
                        tile.grid[y][x];
                }
            }
            i++;
        }
    }

    return m_pixels;
}

const auto& Sprite::create2DSprite(SpriteTileset_t& sprite_tiles)
{
    /*
    uint16_t startTile = objr2->tileNumber;

    for (uint8_t y = 0; y < sizeY / 8; y++) {
        uint16_t x_temp = startTile;
        for (uint8_t x = 0; x < sizeX / 8; x++) {
            uint16_t yy = (x_temp / 32) % 32;
            uint16_t xx = (x_temp) % 32;
            tiles.push_back(_tileset.objTileset[yy][xx]);
            x_temp++;
        }
        startTile += 32;
    }
        */
    return m_pixels;
}

const sprite_t& Sprite::create(SpriteTileset_t& sprite_tiles)
{
    if (displayCtrl->objectVRAMmap) {
        return create1DSprite(sprite_tiles);
    } else {
        return create2DSprite(sprite_tiles);
    }
}

bool Sprite::is_enabled() const
{
    const ObjReg1* objr =
        reinterpret_cast<const ObjReg1*>(oamRam.getMemoryPtr() + m_index * 8);
    return objr->isDoubleOrNoDisplay;
}

uint32_t Sprite::get_priority() const
{
    const ObjReg2* objr = reinterpret_cast<const ObjReg2*>(
        oamRam.getMemoryPtr() + m_index * 8 + 4);
    return objr->priority;
}
