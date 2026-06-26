#include <cstdint>

#include "Gba-Graphics/Rendermodes/TextMode.h"
#include "Gba-Graphics/Tile/Tileset.h"
#include "Memory/memoryMappedIO.h"
#include "Memory/memoryOps.h"

extern Tileset tileset;
extern RgbaPalette PaletteColours;

void TextMode::draw(finalImageColored& img,
                    const uint8_t regOffset,
                    const uint32_t line,
                    const bool first)
{
    if (line >= 160) {
        return;
    }

    BgCnt* bgCnt = (BgCnt*)&IoRAM[8 + regOffset];
    uint32_t startAddr = bgCnt->bgBaseblock * 0x800;
    const uint32_t tileStartRow =
        bgCnt->is8Bit ? bgCnt->tileBaseBlock * 256 : bgCnt->tileBaseBlock * 512;
    const uint8_t sizeX = bgCnt->hWide ? 64 : 32;
    const uint8_t sizeY = bgCnt->vWide ? 64 : 32;

    size_t scan_pixel = 0;

    for (size_t tile_ctrl = 0; tile_ctrl < 30; tile_ctrl++) {
        const auto tile_ctrl0 =
            (BgTile*)&vram[startAddr + tile_ctrl * 2 + ((line / 8) * 64)];
        const auto tile_num = tile_ctrl0->tileNumber + tileStartRow;
        const auto palette_num = tile_ctrl0->paletteNum;
        const auto& tile = tileset.tileset.linear[tile_num].create(
            palette_num, tile_ctrl0->VerticalFlip, tile_ctrl0->horizontalFlip,
            bgCnt->is8Bit);
        for (size_t x = 0; x < 8; x++) {
            const auto back_drop_color =
                PaletteColours.colorFromIndex(palette_num, 0);

            if (tile.grid[line % 8][x].rawColor == back_drop_color.rawColor) {
                scan_pixel++;
                continue;
            }

            img[line][scan_pixel] = tile.grid[line % 8][x];
            scan_pixel++;
        }
    }

    /*
    for (size_t i = 0; i < 32; i++) {
        for (size_t k = 0; k < 32; k++) {
            BgTile* tileCtrl = (BgTile*)&vram[startAddr];
            auto t = tileset.getTile(tileStartRow + tileCtrl->tileNumber / 32,
                                     tileCtrl->tileNumber % 32,
                                     tileCtrl->paletteNum, bgCnt->is8Bit);
            // backgroundTiles[i][k] =
            //     t.flipVertical(tileCtrl->VerticalFlip)
            //         .flipHorizontal(tileCtrl->horizontalFlip);
            startAddr += 2;
        }
    }

        if (bgCnt->hWide) {
            for (size_t i = 0; i < 32; i++) {
                for (size_t k = 0; k < 32; k++) {
                    BgTile* tileCtrl = (BgTile*)&vram[startAddr];
                    backgroundTiles[i][k + 32] =
                        tileset
                            .getTile(tileStartRow + tileCtrl->tileNumber /
   32, tileCtrl->tileNumber % 32, tileCtrl->paletteNum, bgCnt->is8Bit)
                            .flipVertical(tileCtrl->VerticalFlip)
                            .flipHorizontal(tileCtrl->horizontalFlip);
                    startAddr += 2;
                }
            }
        }
        if (bgCnt->vWide) {
            for (size_t i = 0; i < 32; i++) {
                for (size_t k = 0; k < 32; k++) {
                    BgTile* tileCtrl = (BgTile*)&vram[startAddr];
                    backgroundTiles[i + 32][k] =
                        tileset
                            .getTile(tileStartRow + tileCtrl->tileNumber /
   32, tileCtrl->tileNumber % 32, tileCtrl->paletteNum, bgCnt->is8Bit)
                            .flipVertical(tileCtrl->VerticalFlip)
                            .flipHorizontal(tileCtrl->horizontalFlip);
                    startAddr += 2;
                }
            }
        }
        if (bgCnt->vWide && bgCnt->hWide) {
            for (size_t i = 0; i < 32; i++) {
                for (size_t k = 0; k < 32; k++) {
                    BgTile* tileCtrl = (BgTile*)&vram[startAddr];
                    backgroundTiles[i + 32][k + 32] =
                        tileset
                            .getTile(tileStartRow + tileCtrl->tileNumber /
   32, tileCtrl->tileNumber % 32, tileCtrl->paletteNum, bgCnt->is8Bit)
                            .flipVertical(tileCtrl->VerticalFlip)
                            .flipHorizontal(tileCtrl->horizontalFlip);
                    startAddr += 2;
                }
            }
        }

        for (size_t tileY = 0; tileY < sizeY; tileY++) {
            for (size_t pixelY = 0; pixelY < 8; pixelY++) {
                for (size_t tileX = 0; tileX < sizeX; tileX++) {
                    for (size_t pixelX = 0; pixelX < 8; pixelX++) {
                        auto& tile =
                            backgroundTiles[tileY][tileX].grid[pixelY][pixelX];
                        background[8 * tileY + pixelY][8 * tileX + pixelX] =
       tile;
                    }
                }
            }
        }
        */
}

void TextMode::fillImage(finalImagePalettes& imageBase, const uint32_t offset)
{
    /*
    BgCnt* bgCnt = (BgCnt*)&IoRAM[8 + offset];
    uint8_t sizeX = bgCnt->hWide ? 64 : 32;
    uint8_t sizeY = bgCnt->vWide ? 64 : 32;

    uint16_t offsetX = ((BGoffset*)&IoRAM[0x10 + offset * 2])->offset;
    uint16_t offsetY = ((BGoffset*)&IoRAM[0x12 + offset * 2])->offset;
    for (size_t k = 0; k < 160; k++) {
        for (size_t i = 0; i < 240; i++) {
            auto& tile = backgroundTiles[((k + offsetY) / 8) % sizeY]
                                        [((i + offsetX) / 8) % sizeX];
            auto& clr = tile.grid[(k + offsetY) % 8][(i + offsetX) % 8];
            if (imageBase[k][i].index != 0xFFFF && clr.index == 0) {
                continue;
            }
            imageBase[k][i] = clr;
        }
    }
    */
}

uint32_t* TextMode::getBG()
{
    /*
    for (size_t pixelY = 0; pixelY < 64 * 8; pixelY++) {
        for (size_t pixelX = 0; pixelX < 64 * 8; pixelX++) {
            auto t = background[pixelY][pixelX];
            auto clr = PaletteColours.colorFromIndex(t.palette, t.index);
            backgroundColored[pixelY][pixelX] = clr.rawColor;
        }
    }
    return (uint32_t*)backgroundColored;
    */
}
