#include <cstdint>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>

#include "Arm/Swi.hpp"
#include "Display/Disassembler.hpp"
#include "Thumb/ThumbOpcodes/AddSubstract.hpp"
#include "Thumb/ThumbOpcodes/AddToSp.hpp"
#include "Thumb/ThumbOpcodes/AluOps.hpp"
#include "Thumb/ThumbOpcodes/BranchLink.hpp"
#include "Thumb/ThumbOpcodes/ConditionalBranch.hpp"
#include "Thumb/ThumbOpcodes/HiRegOps.hpp"
#include "Thumb/ThumbOpcodes/LoadAddress.hpp"
#include "Thumb/ThumbOpcodes/LoadStoreHalfword.hpp"
#include "Thumb/ThumbOpcodes/LoadStoreImm.hpp"
#include "Thumb/ThumbOpcodes/LoadStoreRegOffset.hpp"
#include "Thumb/ThumbOpcodes/LoadStoreSignExtend.hpp"
#include "Thumb/ThumbOpcodes/MovCmpAddSubImm.hpp"
#include "Thumb/ThumbOpcodes/MoveShiftedRegister.hpp"
#include "Thumb/ThumbOpcodes/MultipleLoad.hpp"
#include "Thumb/ThumbOpcodes/MultipleStore.hpp"
#include "Thumb/ThumbOpcodes/PcRelativeLoad.hpp"
#include "Thumb/ThumbOpcodes/PopRegisters.hpp"
#include "Thumb/ThumbOpcodes/PushRegisters.hpp"
#include "Thumb/ThumbOpcodes/SpRelativeOps.hpp"
#include "Thumb/ThumbOpcodes/UnconditionalBranch.hpp"
#include <Memory/memoryOps.h>


Disassembler::Disassembler()
{
	m_font.loadFromFile("GBA-emulator/arial.ttf");
	m_text.setFont(m_font);
	m_text.setCharacterSize(15);
	m_text.setFillColor(sf::Color::White);
}

std::string Disassembler::thumb_disassembly(const uint32_t program_counter, const uint16_t opcode)
{
	if (AddSubThumb::isThisOpcode(opcode))
		return AddSubThumb::disassemble(opcode);
	else if (MoveShiftedRegister::isThisOpcode(opcode))
		return MoveShiftedRegister::disassemble(opcode);
	else if (MovCmpAddSubImm::isThisOpcode(opcode))
		return MovCmpAddSubImm::disassemble(opcode);
	else if (AluOps::isThisOpcode(opcode))
		return AluOps::disassemble(opcode);
	else if (HighRegOps::isThisOpcode(opcode))
		return HighRegOps::disassemble(opcode);
	else if (PcRelativeLoad::isThisOpcode(opcode))
		return PcRelativeLoad::disassemble(opcode);
	else if (PopRegisters::isThisOpcode(opcode))
		return PopRegisters::disassemble(opcode);
	else if (PushRegisters::isThisOpcode(opcode))
		return PushRegisters::disassemble(opcode);
	else if (LoadStoreReg::isThisOpcode(opcode))
		return LoadStoreReg::disassemble(opcode);
	else if (LoadStoreImm::isThisOpcode(opcode))
		return LoadStoreImm::disassemble(opcode);
	else if (SpRelativeOps::isThisOpcode(opcode))
		return SpRelativeOps::disassemble(opcode);
	else if (AddToSp::isThisOpcode(opcode))
		return AddToSp::disassemble(opcode);
	else if (LoadAddress::isThisOpcode(opcode))
		return LoadAddress::disassemble(opcode);
	else if (LoadStoreHalfword::isThisOpcode(opcode))
		return LoadStoreHalfword::disassemble(opcode);
	else if (LoadStoreSignExtend::isThisOpcode(opcode))
		return LoadStoreSignExtend::disassemble(opcode);
	else if (MultipleLoad::isThisOpcode(opcode))
		return MultipleLoad::disassemble(opcode);
	else if (MultipleStore::isThisOpcode(opcode))
		return MultipleStore::disassemble(opcode);
	else if (ConditionalBranch::isThisOpcode(opcode))
		return ConditionalBranch::disassemble(program_counter, opcode);
	else if (UnconditionalBranch::isThisOpcode(opcode))
		return UnconditionalBranch::disassemble(program_counter, opcode);
	else
		return BranchLink::disassemble(program_counter, opcode);
}

void Disassembler::Show_Registers(const Registers& regs)
{
	const auto output = std::format("R0: {:08x}\nR1: {:08x}\nR2: {:08x}\nR3: {:08x}\n"
                                     "R4: {:08x}\nR5: {:08x}\nR6: {:08x}\nR7: {:08x}\n"
                                     "R8: {:08x}\nR9: {:08x}\nR10: {:08x}\nFP: {:08x}\n"
                                     "IP: {:08x}\n SP: {:08x}\n LR: {:08x}\n PC: {:08x}\n"
                                     "SPSR: {:08x}\n CPSR : {:08x}",
									regs[0],
									regs[1],
									regs[2],
									regs[3],
									regs[4],
									regs[5],
									regs[6],
									regs[7],
									regs[8],
									regs[9],
									regs[10],
									regs[11],
									regs[12],
									regs[13],
									regs[14],
									regs[15],
									regs[16],
									regs.m_cpsr.val);
	m_text.setString(output);
	m_text.setPosition(sf::Vector2f(700, 21));
	m_display->draw(m_text);

}

std::string Disassembler::read_opcodes(const uint32_t program_counter)
{
	std::string output = "";
	uint32_t startpos = program_counter + m_relative_pc_offset;
	for (size_t i = 0; i < 20; i++)
	{
		output += std::format("{:x}   ", startpos);
		const uint32_t opcode = loadFromAddress16(startpos, true);
		output += std::format("{:04x}      ", opcode);
		output += thumb_disassembly(startpos, opcode);
		output += '\n';
		startpos += 2;
	}
	m_text.setString(output);
	m_text.setPosition(sf::Vector2f(16, 21));
	m_display->draw(m_text);

	return output;
}

void Disassembler::display(const Registers& regs)
{
	handleEvents();
	m_display->clear(sf::Color::Black);
	Show_Registers(regs);
	read_opcodes(regs[EProgramCounter]);
	
	m_display->display();
}

void Disassembler::display_disassembly(const Registers& regs)
{
	static bool was_in_focus = true;
	if (!m_in_focus && was_in_focus)
	{
		display(regs);
		was_in_focus = false;
		m_relative_pc_offset = 0;
		return;
	}
	while(m_in_focus)
	{
		display(regs);
		was_in_focus = true;
		if (m_step)
			break;
	}
	m_step = false;
}

void Disassembler::handleEvents()
{
	sf::Event event;
	static bool once_g = true;
	while (m_display->pollEvent(event))
	{
		if (event.type == sf::Event::LostFocus)
		{
			m_in_focus = false;
			return;
		}
		if (event.type == sf::Event::GainedFocus)
		{
			m_in_focus = true;
		}
		if (event.type == sf::Event::MouseButtonReleased)
		{
			uint32_t Mx = sf::Mouse::getPosition(*m_display).x;
			uint32_t My = sf::Mouse::getPosition(*m_display).y;
			if (My < 63)
			{
				m_input = "";
			}
		}

		if (event.type == sf::Event::MouseWheelMoved)
		{
			if (event.mouseWheel.delta <= -1)
			{
				m_relative_pc_offset += 12;
			}
			if (event.mouseWheel.delta >= 1)
			{
				m_relative_pc_offset -= 12;
			}
		}

		if (event.type == sf::Event::Closed)
			m_display->close();

		if (event.type == sf::Event::TextEntered)
		{
			if (m_input.size() > 7) {}
			else if (event.text.unicode > 0x2f && event.text.unicode < 0x3a)
			{
				m_input += static_cast<char>(event.text.unicode);
			}
			else if (event.text.unicode > 0x60 && event.text.unicode < 0x67)
			{
				m_input += static_cast<char>(event.text.unicode);
			}
		}

		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Enter)
			{
				m_relative_pc_offset = std::stoul(m_input, nullptr, 16);
				m_input = "";
			}

			if (event.key.code == sf::Keyboard::Backspace)
			{
				if (m_input.size() > 0)
				{
					m_input = m_input.substr(0, m_input.size() - 1);
				}
			}

			if (event.key.code == sf::Keyboard::G)
			{
				if (once_g)
					once_g = false;
				m_step = true;
			}
		}

		else if (event.type == sf::Event::KeyReleased)
		{
			if (event.key.code == sf::Keyboard::G)
			{
				if (!once_g)
					once_g = true;
				m_step = false;
			}
		}
	}
}