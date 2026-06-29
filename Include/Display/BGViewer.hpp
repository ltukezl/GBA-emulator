#ifndef BGVIEWER_HPP
#define BGVIEWER_HPP

#include <memory>
#include <SFML/Graphics.hpp>

#include "Gba-Graphics/Palette/RgbaPalette.h"

class Tileset;
union BgCnt;

class BGViewer
{
public:

    BGViewer(Tileset& tileset);

    void draw();

private:

    void updateBG(const BgCnt* bgCnt, sf::Texture& res);

    Tileset& m_tileset;

    std::unique_ptr<sf::RenderWindow> m_display =
        std::make_unique<sf::RenderWindow>(
            sf::VideoMode(sf::Vector2u(512 * 4, 512)), "BGViewer");

    sf::Texture m_bg0{sf::Vector2u(512, 512), true};
    sf::Texture m_bg1{sf::Vector2u(512, 512), true};
    sf::Texture m_bg2{sf::Vector2u(512, 512), true};
    sf::Texture m_bg3{sf::Vector2u(512, 512), true};

    sf::Sprite gameSprite0{m_bg0};
    sf::Sprite gameSprite1{m_bg1};
    sf::Sprite gameSprite2{m_bg2};
    sf::Sprite gameSprite3{m_bg3};

    std::unique_ptr<std::array<std::array<RgbaPalette::GBAColor, 512>, 512>>
        m_pixels;
};

#endif
