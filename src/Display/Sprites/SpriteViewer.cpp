#include "Display/SpriteViewer.hpp"
#include "Gba-Graphics/Sprites/SpriteSet.h"

SpriteViewer::SpriteViewer(SpriteSet& spriteset) : m_spriteset{spriteset}
{ gameSprite0.setPosition(sf::Vector2f(64 * 0, 0)); }

void SpriteViewer::display()
{
    handleEvents();
    m_display->clear(sf::Color::Black);

    sprite0.update(
        reinterpret_cast<const uint8_t*>(m_spriteset.m_sprite_set.linear[0]
                                             .create(m_spriteset.m_tileset)
                                             .data()));

    m_display->draw(gameSprite0);

    m_display->display();
}

void SpriteViewer::handleEvents()
{
    const auto onClose = [this](const sf::Event::Closed&) {
        m_display->close();
    };

    m_display->handleEvents(onClose);
}
