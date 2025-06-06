#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <cstdint>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <string>

class Registers;

class Disassembler
{
public:
	Disassembler();
	void display_disassembly(const Registers& regs);
	void handleEvents();

private:
	void display(const Registers& regs);
	void Show_Registers(const Registers& regs);
	std::string thumb_disassembly(const uint32_t program_counter, const uint16_t opcode);
	std::string read_opcodes(const uint32_t program_counter);

	sf::Font m_font;
	sf::Text m_text;

	std::string m_input = "";
	uint32_t m_relative_pc_offset = 0;

	std::unique_ptr<sf::RenderWindow> m_display = std::make_unique<sf::RenderWindow>(sf::VideoMode(1000, 800), "Memory Viewer");

	bool m_in_focus = false;
	bool m_step = false;
};

#endif