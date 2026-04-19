#ifndef GAME_H
#define GAME_H

#include <memory>
#include <SFML/Graphics.hpp>

#include "Gba-Graphics/BGLayer/BGLayer.hpp"
#include "Gba-Graphics/Rendermodes/RenderMode3.h"
#include "Gba-Graphics/Rendermodes/RenderMode4.h"
#include "Gba-Graphics/Rendermodes/TextMode.h"
#include "Memory/memoryOps.h"

class GameDisplay
{
public:
    GameDisplay()
    {
        m_bg1.update(reinterpret_cast<uint8_t*>(&(*m_bgLayer1.pixels)[0][0]));
        m_bg2.update(reinterpret_cast<uint8_t*>(&(*m_bgLayer2.pixels)[0][0]));
        m_bg3.update(reinterpret_cast<uint8_t*>(&(*m_bgLayer3.pixels)[0][0]));
        m_bg4.update(reinterpret_cast<uint8_t*>(&(*m_bgLayer4.pixels)[0][0]));
    }

    void draw();

    void drawLine(uint8_t& LYC)
    {
        if (displayCtrl->bgMode == 0) {
            TextMode::draw(*m_bgLayer2.pixels, 0, LYC, true);
        }
        if (displayCtrl->bgMode == 3) {
            RenderMode3::draw(*m_bgLayer2.pixels, LYC);
        }
        if (displayCtrl->bgMode == 4) {
            RenderMode4::draw(*m_bgLayer2.pixels, LYC);
        }
    }

    void handleEvents();
private:
    std::unique_ptr<sf::RenderWindow> m_display =
        std::make_unique<sf::RenderWindow>(
            sf::VideoMode(sf::Vector2u(240, 160)), "Game");

    sf::Texture m_bg1{sf::Vector2u(240, 160), true};
    sf::Texture m_bg2{sf::Vector2u(240, 160), true};
    sf::Texture m_bg3{sf::Vector2u(240, 160), true};
    sf::Texture m_bg4{sf::Vector2u(240, 160), true};

    sf::Sprite gameSprite{m_bg2};
    sf::Sprite gameSprite2{m_bg3};

    BGLayer m_bgLayer1{
        1, reinterpret_cast<BgCnt*>(reinterpret_cast<uint16_t*>(&IoRAM[8]))};
    BGLayer m_bgLayer2{
        2, reinterpret_cast<BgCnt*>(reinterpret_cast<uint16_t*>(&IoRAM[10]))};
    BGLayer m_bgLayer3{
        3, reinterpret_cast<BgCnt*>(reinterpret_cast<uint16_t*>(&IoRAM[12]))};
    BGLayer m_bgLayer4{
        4, reinterpret_cast<BgCnt*>(reinterpret_cast<uint16_t*>(&IoRAM[14]))};
};

#endif
