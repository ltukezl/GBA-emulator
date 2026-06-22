#include <iostream>

#include "Gba-Graphics/Sprites/Sprite.h"
#include "Gba-Graphics/Sprites/SpriteSet.h"
#include "Gba-Graphics/Tile/Tile.h"
#include "Memory/memoryMappedIO.h"
#include "Memory/memoryOps.h"
#include <iostream>

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
    for (size_t i = 0; i < dimensions.first * dimensions.second; i++) {
        const auto& tile = sprite_tiles.linear[tile_start + i].create(
            16 + objr2->paletteNumber, false, false, false);
        const uint32_t y_offset = (i / 8);
        const uint32_t x_offset = (i % (dimensions.first));

        std::cout << y_offset << " " << x_offset << "\n";

        for (size_t x = 0; x < 8; x++) {
            for (size_t y = 0; y < 8; y++) {
                m_pixels[y_offset * 8 + y][x_offset * 8 + x] = tile.grid[y][x];
            }
        }
    }

    // const auto& tile = m_tileset
    /*
    uint16_t size = (sizeX / 8) * (sizeY / 8);
    uint16_t startTile = objr2->tileNumber;
    for (uint16_t i = 0; i < size; i++) {
        tiles.push_back(_tileset.objTileset[(startTile % 1024) / 32]
                                           [(startTile % 1024) % 32]);
        startTile++;
    }
        */
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
    uint8_t* object_data_address = vram.getMemoryPtr() + 0x10000;
    if (displayCtrl->bgMode == 3 || displayCtrl->bgMode == 4 ||
        displayCtrl->bgMode == 5) {
        object_data_address += 0x4000;
    }

    if (displayCtrl->objectVRAMmap) {
        return create1DSprite(sprite_tiles);
    } else {
        return create2DSprite(sprite_tiles);
    }
}
