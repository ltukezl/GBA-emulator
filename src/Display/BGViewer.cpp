#include "Display/BGViewer.hpp"
#include "Memory/memoryMappedIO.h"
#include "Memory/memoryOps.h"

BGViewer::BGViewer()
{
    gameSprite0.setPosition(sf::Vector2f(512 * 0, 0));
    gameSprite1.setPosition(sf::Vector2f(512 * 1, 0));
    gameSprite2.setPosition(sf::Vector2f(512 * 2, 0));
    gameSprite3.setPosition(sf::Vector2f(512 * 3, 0));

    m_pixels = std::make_unique<
        std::array<std::array<RgbaPalette::GBAColor, 512>, 512>>();
}

#include <chrono>
#include <functional>
#include <iostream>

// Benchmarking function
template<typename Func, typename... Args>
auto benchmark(Func&& func, Args&&... args)
{
    using namespace std::chrono;

    auto start = high_resolution_clock::now();

    // Execute the function with provided arguments
    auto result =
        std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();

    std::cout << "Execution time: " << duration << " µs" << std::endl;

    return result; // Return the function’s result
}

void BGViewer::draw()
{

    auto addr = 0;

    BgCnt* bgCnt = (BgCnt*)&IoRAM[8 + 2];
    BgCnt* bgCnt1 = (BgCnt*)&IoRAM[8 + 2];
    BgCnt* bgCnt2 = (BgCnt*)&IoRAM[8 + 4];
    BgCnt* bgCnt3 = (BgCnt*)&IoRAM[8 + 6];
    uint32_t startAddr = bgCnt->bgBaseblock * 0x800;
    const uint32_t tileStartRow =
        bgCnt->is8Bit ? bgCnt->tileBaseBlock * 8 : bgCnt->tileBaseBlock * 512;
    const uint8_t sizeX = bgCnt->hWide ? 64 : 32;
    const uint8_t sizeY = bgCnt->vWide ? 64 : 32;

    for (size_t i = 0; i < 32; i++) {
        for (size_t k = 0; k < 32; k++) {
            BgTile* tileCtrl = (BgTile*)&vram[startAddr];
            const auto& t =
                m_tileset.tileset.linear[tileStartRow + tileCtrl->tileNumber]
                    .create(tileCtrl->paletteNum, 0, 0, 0);
            for (size_t px_y = 0; px_y < 8; px_y++) {
                for (size_t px_x = 0; px_x < 8; px_x++) {
                    (*m_pixels)[i * 8 + px_y][k * 8 + px_x] =
                        t.grid[px_y][px_x];
                }
            }
            startAddr += 2;
        }
    }

    m_bg0.update(reinterpret_cast<const uint8_t*>(m_pixels->data()));

    m_display->clear(sf::Color::Black);
    m_display->draw(gameSprite0);
    m_display->draw(gameSprite1);
    m_display->draw(gameSprite2);
    m_display->draw(gameSprite3);
    m_display->display();
}
