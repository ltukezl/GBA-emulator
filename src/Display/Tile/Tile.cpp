#include <iostream>

#include "Gba-Graphics/Tile/Tile.h"
#include "Memory/memoryMappedIO.h"
#include "Memory/memoryOps.h"

extern RgbaPalette PaletteColours;

const Tile::GBATile& Tile::create(const uint32_t addr,
                                  const uint8_t paletteNum,
                                  const bool flipV,
                                  const bool flipH,
                                  const bool is8bit)
{
    uint32_t startAddr = 0;
    std::array<uint8_t, 32 * 32 * 2> local;
    std::copy(vram.getMemoryPtr() + addr, vram.getMemoryPtr() + addr + 32,
              local.begin());
    if (flipV == false && flipH == false) {
        for (size_t y = 0; y < 8; y++) {
            uint32_t row = rawLoad32(local.data(), startAddr);

            for (size_t pixel = 0; pixel < 8; pixel++) {
                const uint16_t color = (row & 0xf);
                m_tile.grid[y][pixel] =
                    PaletteColours.colorFromIndex(paletteNum, color);
                row >>= 4;
            }
            startAddr += 4;
        }
        m_regular = true;
        return m_tile;
    }

    /*
    uint32_t startAddr = 0;
    m_fDone = false;
    m_hDone = false;
    m_fhDone = false;

    // create full 4 bit tile and half of 8 bit tile
    for (size_t y = 0; y < 4; y++) {
        uint32_t row1 = rawLoad32(vram.getMemoryPtr(), addr + startAddr);
        uint32_t row2 = rawLoad32(vram.getMemoryPtr(), addr + startAddr + 4);
        const uint8_t px_8bit1 = (row1 >> 0) & 0xFF;
        const uint8_t px_8bit2 = (row1 >> 1) & 0xFF;
        const uint8_t px_8bit3 = (row1 >> 2) & 0xFF;
        const uint8_t px_8bit4 = (row1 >> 3) & 0xFF;
        const uint8_t px_8bit5 = (row2 >> 0) & 0xFF;
        const uint8_t px_8bit6 = (row2 >> 1) & 0xFF;
        const uint8_t px_8bit7 = (row2 >> 2) & 0xFF;
        const uint8_t px_8bit8 = (row2 >> 3) & 0xFF;

        m_tile8Bit.grid[y][0].index = px_8bit1;
        m_tile8Bit.grid[y][1].index = px_8bit2;
        m_tile8Bit.grid[y][2].index = px_8bit3;
        m_tile8Bit.grid[y][3].index = px_8bit4;
        m_tile8Bit.grid[y][4].index = px_8bit5;
        m_tile8Bit.grid[y][5].index = px_8bit6;
        m_tile8Bit.grid[y][6].index = px_8bit7;
        m_tile8Bit.grid[y][7].index = px_8bit8;

        for (size_t pixel = 0; pixel < 8; pixel++) {
            const uint16_t color = (row1 & 0xf);
            m_tile.grid[y * 2][pixel].index = color;
            row1 >>= 4;
        }
        for (size_t pixel = 0; pixel < 8; pixel++) {
            const uint16_t color = (row2 & 0xf);
            m_tile.grid[y * 2 + 1][pixel].index = color;
            row2 >>= 4;
        }
        startAddr += 8;
    }

    for (size_t y = 4; y < 8; y++) {
        for (size_t pixel = 0; pixel < 8; pixel++) {
            const uint8_t color =
                rawLoad8(vram.getMemoryPtr(), addr + startAddr);
            m_tile8Bit.grid[y][pixel].index = color;
            startAddr++;
        }
    }
        */
}

/*
GBATile& Tile::flipVertical(const bool flip)
{
    if (flip) {
        GBATile tmp = {};
        std::copy(linear.begin(), linear.end(), tmp.linear.begin());
        for (int i = 0; i < 8; i++) {
            for (int k = 0; k < 8; k++) {
                grid[i][k] = tmp.grid[7 - i][k];
            }
        }
    }
    return *this;
}

GBATile& Tile::flipHorizontal(const bool flip)
{
    if (flip) {
        GBATile tmp = {};
        std::copy(linear.begin(), linear.end(), tmp.linear.begin());
        for (int i = 0; i < 8; i++) {
            for (int k = 0; k < 8; k++) {
                grid[i][k] = tmp.grid[i][7 - k];
            }
        }
    }
    return *this;
}
*/
