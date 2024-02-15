#include <iostream>
#include <cstring>

#include "Display/MemoryViewer.h"
#include "Memory/memoryOps.h"

MemoryViewer::MemoryViewer() {
	m_font.loadFromFile("GBA-emulator/arial.ttf");
	m_text.setFont(m_font);
	m_text.setCharacterSize(15);
}

void MemoryViewer::createColumnsHeader()
{
	constexpr auto offsetX = 0;
	constexpr auto offsetY = 0;

	for (uint32_t k = 0; k < 0x10; k++)
	{
		char msg[512] = {};
		sprintf_s(msg, "%02X", k);

		m_text.setString(msg);
		m_text.setPosition(sf::Vector2f(170 + 30 * k, 110));
		m_display->draw(m_text);
	}
}

void MemoryViewer::renderMemory() {
	m_display->clear(sf::Color::Black);
	auto bios = memoryLayout[m_selectIndex];
	bios += m_memAddressStartOffset;

	createColumnsHeader();

	sf::RectangleShape line(sf::Vector2f(1000, 3));
	line.setPosition(sf::Vector2f(0, 64));
	m_display->draw(line);

	// mem select box
	for (uint32_t i = 0; i < 16; i++) {
		sf::RectangleShape memArea(sf::Vector2f(62, 62));
		memArea.setPosition(sf::Vector2f(63*i, 1));
		if (i == m_selectIndex)
		{
			memArea.setFillColor(sf::Color(0, 255, 0));
		}
		m_display->draw(memArea);
	}

	sf::RectangleShape textBox(sf::Vector2f(80, 25));
	textBox.setPosition(sf::Vector2f(20, 105));
	m_display->draw(textBox);

	m_text.setColor(sf::Color::Black);
	m_text.setString(m_input);
	m_text.setPosition(sf::Vector2f(25, 107));
	m_display->draw(m_text);

	m_text.setString("Bios");
	m_text.setPosition(sf::Vector2f(16, 21));
	m_display->draw(m_text);

	m_text.setString("Bios");
	m_text.setPosition(sf::Vector2f(78, 21));
	m_display->draw(m_text);

	m_text.setString("EWRAM");
	m_text.setPosition(sf::Vector2f(130, 21));
	m_display->draw(m_text);

	m_text.setString("IWRAM");
	m_text.setColor(sf::Color::Black);
	m_text.setPosition(sf::Vector2f(195, 21));
	m_display->draw(m_text);

	m_text.setString("IORAM");
	m_text.setPosition(sf::Vector2f(260, 21));
	m_display->draw(m_text);

	m_text.setString("Palette");
	m_text.setPosition(sf::Vector2f(322, 21));
	m_display->draw(m_text);

	m_text.setString("VRAM");
	m_text.setPosition(sf::Vector2f(388, 21));
	m_display->draw(m_text);

	m_text.setString("OAM");
	m_text.setPosition(sf::Vector2f(453, 21));
	m_display->draw(m_text);

	m_text.setString("GPAK");
	m_text.setPosition(sf::Vector2f(514, 21));
	m_display->draw(m_text);

	m_text.setString("GPAK");
	m_text.setPosition(sf::Vector2f(576, 21));
	m_display->draw(m_text);

	m_text.setString("GPAK");
	m_text.setPosition(sf::Vector2f(638, 21));
	m_display->draw(m_text);

	m_text.setString("GPAK");
	m_text.setPosition(sf::Vector2f(700, 21));
	m_display->draw(m_text);

	m_text.setString("GPAK");
	m_text.setPosition(sf::Vector2f(762, 21));
	m_display->draw(m_text);

	m_text.setString("GPAK");
	m_text.setPosition(sf::Vector2f(824, 21));
	m_display->draw(m_text);

	m_text.setString("SRAM");
	m_text.setPosition(sf::Vector2f(890, 21));
	m_display->draw(m_text);

	m_text.setString("SRAM");
	m_text.setPosition(sf::Vector2f(954, 21));
	m_display->draw(m_text);
	
	m_text.setColor(sf::Color::White);
	for (uint32_t i = 0; i < 32; i++) {
		char msg[512] = {};

		for (uint32_t k = 0; k < 0x10; k++)
		{
			sprintf_s(msg, "%02X", bios[k]);

			m_text.setString(msg);
			m_text.setPosition(sf::Vector2f(170 + 30*k, 150 + i * 20));
			m_display->draw(m_text);
		}

		for (uint32_t k = 0; k < 0x10; k++)
		{
			// printable char
			if (bios[k] > 0x20 && bios[k] < 0x80) {
				sprintf_s(msg, "%c", bios[k]);
				m_text.setString(msg);
			}
			else {
				m_text.setString('.');
			}
			
			m_text.setPosition(sf::Vector2f(700 + 10 * k, 150 + i * 20));
			m_display->draw(m_text);
		}

		sprintf_s(msg, "0x%08X", m_memAddressStartOffset + 0x10 * i + 0x100'0000 * m_selectIndex);
		m_text.setString(msg);
		m_text.setPosition(sf::Vector2f(20, 150 + i * 20));
		m_display->draw(m_text);
		bios += 0x10;
	}

	m_display->display();
}

void MemoryViewer::handleEvents() {
	sf::Event event;
	while (m_display->pollEvent(event))
	{
		if (event.type == sf::Event::MouseButtonReleased) {
			uint32_t Mx = sf::Mouse::getPosition(*m_display).x;
			uint32_t My = sf::Mouse::getPosition(*m_display).y;
			if (My < 63) {
				m_selectIndex = Mx / 63;
				m_memAddressStartOffset = 0;
				m_input = "";
			}
		}

		if (event.type == sf::Event::MouseWheelMoved)
		{
			if (event.mouseWheel.delta <= -1)
			{
				m_memAddressStartOffset += 0x30;
			}
			if (event.mouseWheel.delta >= 1)
			{
				if (m_memAddressStartOffset >= 0x30) {
					m_memAddressStartOffset -= 0x30;
				}
			}
		}

		if (event.type == sf::Event::Closed)
			m_display->close();

		if (event.type == sf::Event::TextEntered) {
			if (m_input.size() > 7) {}
			else if(event.text.unicode > 0x2f && event.text.unicode < 0x3a) {
				m_input += static_cast<char>(event.text.unicode);
				std::cout << m_input << "\n";
			}
			else if (event.text.unicode > 0x60 && event.text.unicode < 0x67) {
				m_input += static_cast<char>(event.text.unicode);
				std::cout << m_input << "\n";
			}
		}

		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Enter)
			{
				m_memAddressStartOffset = std::stoul(m_input, nullptr, 16);
				m_input = "";
				std::cout << m_input << "\n";
			}

			if (event.key.code == sf::Keyboard::Backspace)
			{
				if (m_input.size() > 0) {
					m_input = m_input.substr(0, m_input.size() - 1);
				}
				std::cout << m_input << "\n";
			}
		}

		else if (event.type == sf::Event::KeyReleased)
		{

		}
	}
}