#ifndef GAME_H
#define GAME_H

#include <array>
#include <memory>
#include <SFML/Graphics.hpp>

#include "Gba-Graphics/BGLayer/BGLayer.hpp"
#include "Gba-Graphics/Rendermodes/RenderMode3.h"
#include "Gba-Graphics/Rendermodes/RenderMode4.h"
#include "Gba-Graphics/Rendermodes/RenderMode5.h"
#include "Memory/memoryOps.h"

class GameDisplay
{
public:
    GameDisplay()
    {
        m_game_pixels = std::make_unique<finalImageColored>();
        m_game_texture.update(
            reinterpret_cast<uint8_t*>(&(*m_game_pixels)[0][0]));
    }

    void draw();

    void drawLine(uint8_t& LYC)
    {
        if (LYC == 0) {
            for (auto& row: *m_game_pixels) {
                const RgbaPalette::GBAColor clr{1};
                row.fill(clr);
            }
        }
        if (displayCtrl->bgMode == 0) {
            std::sort(m_all_bg_layers.begin(), m_all_bg_layers.end(),
                      std::greater<>());
            for (auto& bg_layer: m_all_bg_layers) {
                if (bg_layer.is_enabled()) {
                    bg_layer.draw_text_mode(*m_game_pixels, LYC, false);
                }
            }
        } else if (displayCtrl->bgMode == 3) {
            RenderMode3::draw(*m_game_pixels, LYC);
        } else if (displayCtrl->bgMode == 4) {
            RenderMode4::draw(*m_game_pixels, LYC);
        } else if (displayCtrl->bgMode == 5) {
            RenderMode5::draw(*m_game_pixels, LYC);
        }
    }

    void handleEvents();
private:
    std::unique_ptr<sf::RenderWindow> m_display =
        std::make_unique<sf::RenderWindow>(
            sf::VideoMode(sf::Vector2u(240, 160)), "Game");

    sf::Texture m_game_texture{sf::Vector2u(240, 160), true};

    sf::Sprite m_game_sprite{m_game_texture};

    std::array<BGLayer, 4> m_all_bg_layers{
        {{0, reinterpret_cast<BgCnt*>(reinterpret_cast<uint16_t*>(&IoRAM[8]))},
         {1, reinterpret_cast<BgCnt*>(reinterpret_cast<uint16_t*>(&IoRAM[10]))},
         {2, reinterpret_cast<BgCnt*>(reinterpret_cast<uint16_t*>(&IoRAM[12]))},
         {3,
          reinterpret_cast<BgCnt*>(reinterpret_cast<uint16_t*>(&IoRAM[14]))}},
    };

    BGLayer& m_bgLayer2 = m_all_bg_layers[1];
    std::unique_ptr<finalImageColored> m_game_pixels;
};

#endif
