#include "Display/BGViewer.hpp"
#include "Memory/memoryMappedIO.h"
#include "Memory/memoryOps.h"

void BGViewer::updateBG(const BgCnt* bgCnt, sf::Texture& res)
{
    for (auto& row: *m_pixels) {
        const RgbaPalette::GBAColor clr{0};
        row.fill(clr);
    }
    uint32_t startAddr = bgCnt->bgBaseblock * 0x800;
    const uint32_t tileStartRow =
        bgCnt->is8Bit ? bgCnt->tileBaseBlock * 256 : bgCnt->tileBaseBlock * 512;
    const bool sizeX = bgCnt->hWide;
    const bool sizeY = bgCnt->vWide;

    for (size_t i = 0; i < 32; i++) {
        for (size_t k = 0; k < 32; k++) {
            BgTile* tileCtrl = (BgTile*)&vram[startAddr];
            const auto& t =
                m_tileset.tileset.linear[tileStartRow + tileCtrl->tileNumber]
                    .create(tileCtrl->paletteNum, tileCtrl->VerticalFlip,
                            tileCtrl->horizontalFlip, bgCnt->is8Bit);
            for (size_t px_y = 0; px_y < 8; px_y++) {
                for (size_t px_x = 0; px_x < 8; px_x++) {
                    (*m_pixels)[i * 8 + px_y][k * 8 + px_x] =
                        t.grid[px_y][px_x];
                }
            }
            startAddr += 2;
        }
    }

    if (sizeX) {
        for (size_t i = 0; i < 32; i++) {
            for (size_t k = 0; k < 32; k++) {
                BgTile* tileCtrl = (BgTile*)&vram[startAddr];
                const auto& t =
                    m_tileset.tileset
                        .linear[tileStartRow + tileCtrl->tileNumber]
                        .create(tileCtrl->paletteNum, tileCtrl->VerticalFlip,
                                tileCtrl->horizontalFlip, bgCnt->is8Bit);
                for (size_t px_y = 0; px_y < 8; px_y++) {
                    for (size_t px_x = 0; px_x < 8; px_x++) {
                        (*m_pixels)[i * 8 + px_y][256 + k * 8 + px_x] =
                            t.grid[px_y][px_x];
                    }
                }
                startAddr += 2;
            }
        }
    }

    res.update(reinterpret_cast<const uint8_t*>(m_pixels->data()));
}

BGViewer::BGViewer()
{
    gameSprite0.setPosition(sf::Vector2f(512 * 0, 0));
    gameSprite1.setPosition(sf::Vector2f(512 * 1, 0));
    gameSprite2.setPosition(sf::Vector2f(512 * 2, 0));
    gameSprite3.setPosition(sf::Vector2f(512 * 3, 0));

    m_pixels = std::make_unique<
        std::array<std::array<RgbaPalette::GBAColor, 512>, 512>>();
}

void BGViewer::draw()
{
    BgCnt* bgCnt = (BgCnt*)&IoRAM[8 + 0];
    BgCnt* bgCnt1 = (BgCnt*)&IoRAM[8 + 2];
    BgCnt* bgCnt2 = (BgCnt*)&IoRAM[8 + 4];
    BgCnt* bgCnt3 = (BgCnt*)&IoRAM[8 + 6];

    updateBG(bgCnt, m_bg0);
    updateBG(bgCnt1, m_bg1);
    updateBG(bgCnt2, m_bg2);
    updateBG(bgCnt3, m_bg3);

    m_display->clear(sf::Color::Black);
    m_display->draw(gameSprite0);
    m_display->draw(gameSprite1);
    m_display->draw(gameSprite2);
    m_display->draw(gameSprite3);
    m_display->display();
}
