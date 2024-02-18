#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <cstring>

#include "Memory/memoryMappedIO.h"

extern RgbaPalette PaletteColours;

class PaletteViewer {
public:
	PaletteViewer() {
		m_font.loadFromFile("GBA-emulator/arial.ttf");
		m_text.setFont(m_font);
		m_text.setCharacterSize(15);

		m_paletteTexture.create(16, 32);

		m_paletteSprite.setTexture(m_paletteTexture, true);
		m_paletteSprite.setPosition(0, 0);
		m_paletteSprite.setScale(16.0, 16.0);
	}

	void renderPalettes() {
		m_display->clear(sf::Color::Black);
		PaletteColours.updatePalette();
		m_paletteTexture.update(PaletteColours.getPalette());

		m_display->draw(m_paletteSprite);
		m_display->display();
	}

	void handleEvents() {
		sf::Event event;
		while (m_display->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				m_display->close();
		}
	}

private:
	sf::Sprite m_paletteSprite;
	sf::Texture m_paletteTexture;
	sf::Font m_font;
	sf::Text m_text;
	std::unique_ptr<sf::RenderWindow> m_display = std::make_unique<sf::RenderWindow>(sf::VideoMode(512, 512), "PaletteViewer");
};