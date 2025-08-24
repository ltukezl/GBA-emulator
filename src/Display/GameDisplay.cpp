#include "Display/GameDisplay.hpp"

void GameDisplay::draw()
{
    m_bg2.update(reinterpret_cast<uint8_t*>(&(*m_bgLayer2.pixels)[0][0]));
    gameSprite.setTexture(m_bg2);
    m_display->clear(sf::Color::Black);
    m_display->draw(gameSprite);
    // m_display->draw(gameSprite2);
    m_display->display();
}
