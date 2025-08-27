#include "Display/BGViewer.hpp"

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

    const auto& t = m_tileset.tileset.grid[0][0].create(0x4400, 0, 0, 0, 0);

    for (size_t i = 0; i < 8; i++) {
        for (size_t k = 0; k < 8; k++) {
            (*m_pixels)[i][k] = t.grid[i][k];
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
