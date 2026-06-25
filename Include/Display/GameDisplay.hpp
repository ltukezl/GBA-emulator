#ifndef GAME_H
#define GAME_H

#include <array>
#include <memory>
#include <SFML/Graphics.hpp>
#include <variant>

#include "Gba-Graphics/BGLayer/BGLayer.hpp"
#include "Gba-Graphics/Rendermodes/RenderMode3.h"
#include "Gba-Graphics/Rendermodes/RenderMode4.h"
#include "Gba-Graphics/Rendermodes/RenderMode5.h"
#include "Gba-Graphics/Sprites/SpriteSet.h"
#include "Include/Gba-Graphics/GBADrawable.hpp"
#include "Memory/memoryOps.h"

extern SpriteSet spriteset;

class GameDisplay
{
public:
    using RenderItem = std::variant<const Sprite*, const BGLayer*>;
    GameDisplay()
    {
        size_t i = 0;
        for (auto& obj: m_all_bg_layers) {
            m_all_drawables[i] = &obj;
            i++;
        }
        for (auto& obj: spriteset.m_sprite_set.linear) {
            m_all_drawables[i] = &obj;
            i++;
        }
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
            auto comparison_func = [](const auto& a, const auto& b) {
                std::visit(
                    [](const auto a, const auto b) {
                        return *a < *b;
                    },
                    a, b);
                return false;
            };

            std::sort(m_all_drawables.begin(), m_all_drawables.end(),
                      comparison_func);
            for (const auto& drawable: m_all_drawables) {
                if (std::holds_alternative<const BGLayer*>(drawable)) {
                    auto bg_layer = std::get<const BGLayer*>(drawable);
                    if (bg_layer->is_enabled()) {
                        bg_layer->draw_text_mode(*m_game_pixels, LYC, false);
                    }
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

    std::array<RenderItem, 4 + 128> m_all_drawables;

    std::unique_ptr<finalImageColored> m_game_pixels;
};

#endif
