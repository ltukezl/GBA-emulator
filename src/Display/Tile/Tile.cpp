#include <immintrin.h>
#include <iostream>

#include "Gba-Graphics/Tile/Tile.h"
#include "Memory/memoryMappedIO.h"
#include "Memory/memoryOps.h"

extern RgbaPalette PaletteColours;

const Tile::GBATile& Tile::create_8bit(const bool flipH, const bool flipV)
{
    const auto tile_addr = vram.getMemoryPtr() + m_idx * 2;
    for (size_t i = 0; i < 64; i++) {
        m_tile.linear[i] = PaletteColours.colorFromIndex(tile_addr[i]);
    }

    for (size_t k = 0; k < 8; k++) {
        for (size_t i = 0; i < 8; i++) {
            m_tileV.grid[k][7 - i] = m_tile.grid[k][i];
        }
    }

    for (size_t k = 0; k < 8; k++) {
        for (size_t i = 0; i < 8; i++) {
            m_tileH.grid[7 - k][i] = m_tile.grid[k][i];
        }
    }

    for (size_t k = 0; k < 8; k++) {
        for (size_t i = 0; i < 8; i++) {
            m_tileHV.grid[7 - k][7 - i] = m_tile.grid[k][i];
        }
    }

    if (flipV == false && flipH == false) {
        return m_tile;
    } else if (flipV == true && flipH == false) {
        return m_tileV;
    } else if (flipV == false && flipH == true) {
        return m_tileH;
    } else {
        return m_tileHV;
    }
}

const Tile::GBATile& Tile::create(const uint8_t paletteNum,
                                  const bool flipH,
                                  const bool flipV,
                                  const bool is8bit)
{
    if (is8bit) {
        return create_8bit(flipH, flipV);
    }
    const auto wholeCurrentPalette =
        _mm256_loadu_si256(reinterpret_cast<__m256i*>(
            paletteram.getMemoryPtr() + 32 * paletteNum));
    const auto vcmp = _mm256_cmpeq_epi32(wholeCurrentPalette, m_lastPalette);
    const uint32_t cmp_mask = _mm256_movemask_epi8(vcmp);
    const bool result = (cmp_mask == 0xffff'ffff);
    // if (!vram.m_observer.checkAccessed(m_idx) and result) {
    //     if (flipV == true && flipH == false) {
    //         return m_tileV;
    //     } else if (flipV == false && flipH == true) {
    //         return m_tileH;
    //     } else if (flipV == true && flipH == true) {
    //         return m_tileHV;
    //     }
    //     return m_tile;
    // }
    m_lastPalette = wholeCurrentPalette;
    vram.m_observer.clearAccessed(m_idx);
    // load row of nibble offsets
    // AB CD EF GH XX XX XX XX
    const auto tmp = _mm256_loadu_si256(
        reinterpret_cast<__m256i*>(vram.getMemoryPtr() + m_idx));

    // shift nibbles
    //  XA BC DE FG HX XX XX XX
    const auto rot = _mm256_srli_epi64(tmp, 4);

    // merge the two nibbles of vectors
    //  XA AB XC CD XD EF XG GH
    const auto idx = _mm256_unpacklo_epi8(tmp, rot);
    const auto idx2 = _mm256_unpackhi_epi8(tmp, rot);

    // Mask nibbles and gather the values
    // 0A 0B 0C 0D 0E 0F 0G 0H
    const auto mask = _mm256_set1_epi8(0x0f);
    const auto masked = _mm256_and_si256(idx, mask);
    const auto masked2 = _mm256_and_si256(idx2, mask);

    alignas(32) uint8_t out[64];
    _mm256_store_si256(reinterpret_cast<__m256i*>(out), masked);
    _mm256_store_si256(reinterpret_cast<__m256i*>(out + 32), masked2);
    uint8_t px = 0;
    for (size_t i = 0; i < 16; i++) {
        m_tile.linear[i] = PaletteColours.colorFromIndex(paletteNum, out[px++]);
    }
    for (size_t i = 0; i < 16; i++) {
        m_tile.linear[i + 32] =
            PaletteColours.colorFromIndex(paletteNum, out[px++]);
    }
    for (size_t i = 0; i < 16; i++) {
        m_tile.linear[i + 16] =
            PaletteColours.colorFromIndex(paletteNum, out[px++]);
    }
    for (size_t i = 0; i < 16; i++) {
        m_tile.linear[i + 48] =
            PaletteColours.colorFromIndex(paletteNum, out[px++]);
    }

    auto tmp0 = _mm256_loadu_si256(
        reinterpret_cast<__m256i*>(m_tile.linear.data() + 0));
    auto tmp1 = _mm256_loadu_si256(
        reinterpret_cast<__m256i*>(m_tile.linear.data() + 8));
    auto tmp2 = _mm256_loadu_si256(
        reinterpret_cast<__m256i*>(m_tile.linear.data() + 16));
    auto tmp3 = _mm256_loadu_si256(
        reinterpret_cast<__m256i*>(m_tile.linear.data() + 24));
    auto tmp4 = _mm256_loadu_si256(
        reinterpret_cast<__m256i*>(m_tile.linear.data() + 32));
    auto tmp5 = _mm256_loadu_si256(
        reinterpret_cast<__m256i*>(m_tile.linear.data() + 40));
    auto tmp6 = _mm256_loadu_si256(
        reinterpret_cast<__m256i*>(m_tile.linear.data() + 48));
    auto tmp7 = _mm256_loadu_si256(
        reinterpret_cast<__m256i*>(m_tile.linear.data() + 56));

    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileH.linear.data() + 0),
                       tmp7);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileH.linear.data() + 8),
                       tmp6);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileH.linear.data() + 16),
                       tmp5);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileH.linear.data() + 24),
                       tmp4);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileH.linear.data() + 32),
                       tmp3);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileH.linear.data() + 40),
                       tmp2);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileH.linear.data() + 48),
                       tmp1);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileH.linear.data() + 56),
                       tmp0);

    tmp0 = _mm256_permutevar8x32_epi32(
        tmp0, _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7));
    tmp1 = _mm256_permutevar8x32_epi32(
        tmp1, _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7));
    tmp2 = _mm256_permutevar8x32_epi32(
        tmp2, _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7));
    tmp3 = _mm256_permutevar8x32_epi32(
        tmp3, _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7));
    tmp4 = _mm256_permutevar8x32_epi32(
        tmp4, _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7));
    tmp5 = _mm256_permutevar8x32_epi32(
        tmp5, _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7));
    tmp6 = _mm256_permutevar8x32_epi32(
        tmp6, _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7));
    tmp7 = _mm256_permutevar8x32_epi32(
        tmp7, _mm256_set_epi32(0, 1, 2, 3, 4, 5, 6, 7));

    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileV.linear.data() + 0),
                       tmp0);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileV.linear.data() + 8),
                       tmp1);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileV.linear.data() + 16),
                       tmp2);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileV.linear.data() + 24),
                       tmp3);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileV.linear.data() + 32),
                       tmp4);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileV.linear.data() + 40),
                       tmp5);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileV.linear.data() + 48),
                       tmp6);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileV.linear.data() + 56),
                       tmp7);

    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileHV.linear.data() + 0),
                       tmp7);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileHV.linear.data() + 8),
                       tmp6);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileHV.linear.data() + 16),
                       tmp5);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileHV.linear.data() + 24),
                       tmp4);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileHV.linear.data() + 32),
                       tmp3);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileHV.linear.data() + 40),
                       tmp2);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileHV.linear.data() + 48),
                       tmp1);
    _mm256_store_si256(reinterpret_cast<__m256i*>(m_tileHV.linear.data() + 56),
                       tmp0);

    if (flipV == false && flipH == false) {
        return m_tile;
    } else if (flipV == true && flipH == false) {
        return m_tileV;
    } else if (flipV == false && flipH == true) {
        return m_tileH;
    } else {
        return m_tileHV;
    }
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
