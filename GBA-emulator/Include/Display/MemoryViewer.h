#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

class MemoryViewer
{
public:
	MemoryViewer();

	void renderMemory();
	void handleEvents();

private:

	void createColumnsHeader();

	sf::Font m_font;
	sf::Text m_text;
	std::unique_ptr<sf::RenderWindow> m_display = std::make_unique<sf::RenderWindow>(sf::VideoMode(1000, 800), "Memory Viewer");
	uint32_t m_memAddressStartOffset = 0;
	uint32_t m_selectIndex = 0;
};