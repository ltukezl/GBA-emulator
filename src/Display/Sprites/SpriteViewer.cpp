#include "Display/SpriteViewer.hpp"
#include "Gba-Graphics/Sprites/SpriteSet.h"

SpriteViewer::SpriteViewer(SpriteSet& spriteset) : m_spriteset{spriteset}
{
    size_t i = 0;
    for (auto& texture: m_textures) {

        texture.resize(sf::Vector2u(64, 64), true);
        m_sprites.emplace_back(texture);
        i++;
    }

    i = 0;
    for (size_t y = 0; y < 16; y++) {
        for (size_t x = 0; x < 8; x++) {
            if (i == 127) {
                return;
            }
            m_sprites[i].setPosition(sf::Vector2f(64 * x, 64 * y));
            i++;
        }
    }
}

void SpriteViewer::display()
{
    handleEvents();
    m_display->clear(sf::Color::Black);

    size_t i = 0;
    for (const auto& sprite: m_sprites) {
        m_textures[i].update(
            reinterpret_cast<const uint8_t*>(m_spriteset.m_sprite_set.linear[i]
                                                 .create(m_spriteset.m_tileset)
                                                 .data()));

        m_display->draw(sprite);
        i++;
    }

    m_display->display();
}

void SpriteViewer::handleEvents()
{
    const auto onClose = [this](const sf::Event::Closed&) {
        m_display->close();
    };

    m_display->handleEvents(onClose);
}
