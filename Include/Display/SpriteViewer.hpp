#pragma once

#include <memory>
#include <SFML/Graphics.hpp>

class SpriteSet;

class SpriteViewer
{
public:

    SpriteViewer(SpriteSet& spriteset);

    void display();
    void handleEvents();

private:

    sf::Texture sprite0{sf::Vector2u(64, 64), true};
    sf::Sprite gameSprite0{sprite0};

    std::unique_ptr<sf::RenderWindow> m_display =
        std::make_unique<sf::RenderWindow>(
            sf::VideoMode(sf::Vector2u(64 * 16, 64 * 8)), "SpriteViewer");

    SpriteSet& m_spriteset;
};
