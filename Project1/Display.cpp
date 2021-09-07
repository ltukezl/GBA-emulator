#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <iostream>
#include "Display.h"
#include "MemoryOps.h"
#include "GBAcpu.h"
#include "Constants.h"
#include "memoryMappedIO.h"

Display::Display(int res_x, int res_y, char* name) : res_x(res_x), res_y(res_y), name(name){
	display = new sf::RenderWindow(sf::VideoMode(res_x, res_y), name);
	txtRing = (Ring*)malloc(sizeof(Ring));
	memset(txtRing, 0, sizeof(Ring));
	Ring* tmp = txtRing;
	for (int i = 0; i < 10; i++){
		tmp->next = (Ring*)malloc(sizeof(Ring));
		tmp = tmp->next;
		memset(tmp, 0, sizeof(Ring));
	}
	tmp->next = txtRing;
}

void Display::scanPalettes(){
	int startAddr = 0x05000000;
	sf::RectangleShape rectangle;
	rectangle.setSize(sf::Vector2f(16, 16));

	const int scalar = 255 / 31;

	//for bg palettes
	for (int i = 0; i < 16; i++)
		for (int k = 0; k < 16; k++){
			ColorPaletteRam.addr = loadFromAddress16(startAddr, true);
			int redScaled = ColorPaletteRam.red * scalar;
			int greenScaled = ColorPaletteRam.green * scalar;
			int blueScaled = ColorPaletteRam.blue * scalar;
			sf::Color color(redScaled, greenScaled, blueScaled);
			PaletteColors[16 * i + k] = color;
			rectangle.setFillColor(color);
			rectangle.setPosition(sf::Vector2f(16 * k, 16 * i));
			display->draw(rectangle);
			startAddr += 2;
		}

	//for fg palettes
	for (int i = 0; i < 16; i++)
		for (int k = 0; k < 16; k++){
			ColorPaletteRam.addr = loadFromAddress16(startAddr, true);
			int redScaled = ColorPaletteRam.red * scalar;
			int greenScaled = ColorPaletteRam.green * scalar;
			int blueScaled = ColorPaletteRam.blue * scalar;
			sf::Color color(redScaled, greenScaled, blueScaled);
			PaletteColors[256 + 16 * i + k] = color;
			rectangle.setFillColor(color);
			rectangle.setPosition(sf::Vector2f(16 * k, 272 + 16 * i));
			display->draw(rectangle);
			startAddr += 2;
		}
}

void Display::fillTiles(uint32_t regOffset){
	BgCnt.addr = loadFromAddress16(0x4000008 + regOffset, true);
	int startAddr = 0x06000000;

	/*creates tilemaps 1 and 2*/
	sf::Image tile;
	tile.create(8, 8, sf::Color::Blue);
	for (int tileY = 0; tileY < 64; tileY++)
		for (int tileX = 0; tileX < 32; tileX++){
			for (int y = 0; y < 8; y++){
				int row = loadFromAddress32(startAddr, true);
				for (int pixel = 0; pixel < 8; pixel++){
					int color = (row & 0xf);
					tile.setPixel(pixel, y, PaletteColors[color]);
					row >>= 4;
				}
				startAddr += 4;
			}
			tileMap[32 * tileY + tileX] = tile;
		}


	/*draws tilemaps 1 and 2 to screen*/
	sf::Texture tileMap1Texture;
	tileMap1Texture.create(256 * 2, 513);

	for (int i = 0; i < 64; i++)
		for (int k = 0; k < 32; k++){
			sf::Image tile = tileMap[32 * i + k];
			tileMap1Texture.update(tile, 8 * k, 8 * i);
		}

	sf::Sprite tilemapSprite;
	tilemapSprite.setTexture(tileMap1Texture, true);
	tilemapSprite.setPosition(257, 0);

	display->draw(tilemapSprite);
}

void Display::fillBG(uint32_t regOffset){
	BgCnt.addr = loadFromAddress16(0x4000008 + regOffset, true);
	uint32_t startAddr = 0x06000000 + BgCnt.bgBaseblock * 2048;
	uint32_t tileBaseBlock = BgCnt.tileBaseBlock * 0x200;
	/*fills BG map*/
	sf::Texture BG1Texture;
	BG1Texture.create(256, 256);
	for (int i = 0; i < 32; i++){
		for (int k = 0; k < 32; k++){
			uint16_t reg = loadFromAddress16(startAddr, true);
			uint16_t tilNum = reg & 0x1FF;
			BG1Texture.update(tileMap[tilNum + tileBaseBlock], 8 * k, 8 * i);
			startAddr += 2;
		}
	}
	sf::Sprite BG1Sprite;
	BG1Sprite.setTexture(BG1Texture, true);
	BG1Sprite.setPosition(514, 0);

	display->draw(BG1Sprite);
}

void Display::fillObjects(uint32_t regOffset){
	BgCnt.addr = loadFromAddress16(0x4000008 + regOffset, true);
	int startAddr = 0x06010000;

	/*creates tilemaps 1 and 2*/
	sf::Image tile;
	tile.create(8, 8, sf::Color::Blue);
	for (int tileY = 0; tileY < 32; tileY++)
		for (int tileX = 0; tileX < 32; tileX++){
			for (int y = 0; y < 8; y++){
				int row = loadFromAddress32(startAddr, true);
				for (int pixel = 0; pixel < 8; pixel++){
					int color = (row & 0xf);
					tile.setPixel(pixel, y, PaletteColors[256 + color]);
					row >>= 4;
				}
				startAddr += 4;
			}
			objMap[32 * tileY + tileX] = tile;
		}


	/*draws tilemaps 1 and 2 to screen*/
	sf::Texture objMapTexture;
	objMapTexture.create(256, 513);

	for (int i = 0; i < 32; i++)
		for (int k = 0; k < 32; k++){
			sf::Image tile = objMap[32 * i + k];
			objMapTexture.update(tile, 8 * k, 8 * i);
		}

	sf::Sprite objMapSprite;
	objMapSprite.setTexture(objMapTexture, true);
	objMapSprite.setPosition(257*3, 0);

	display->draw(objMapSprite);
}

void Display::updatePalettes(){
	display->clear(sf::Color::Black);

	DISPCNT.addr = loadFromAddress16(0x4000000, true);

	scanPalettes();
	fillObjects(0);

	if (DISPCNT.bg0Display){
		fillTiles(0);
		fillBG(0);
	}
	if (DISPCNT.bg1Display){
		fillTiles(2);
		fillBG(2);
	}
	if (DISPCNT.bg2Display){
		fillTiles(4);
		fillBG(4);
	}
	if (DISPCNT.bg3Display){
		fillTiles(6);
		fillBG(6);
	}
	
#ifdef ENABLED
	InterruptFlagRegister.addr = loadFromAddress16(0x4000202, true);
	InterruptEnableRegister.addr = loadFromAddress16(0x4000200, true);
	if (InterruptEnableRegister.vBlank && LCDstatus.vIRQEn){
		if (DISPCNT.forceBlank)
			InterruptFlagRegister.vBlank = 1;
		rawWrite16(IoRAM, 0x202, InterruptFlagRegister.addr);
	}
#endif
	
	sf::Font font;
	font.loadFromFile("arial.ttf");

	sf::Text text;
	text.setFont(font);
	text.setColor(sf::Color::Red);
	text.setCharacterSize(15);
	text.setStyle(sf::Text::Bold);

	for (int i = 0; i < 10; i++){
		char txt[16];
		_itoa_s((*r[SP]) + i * 4, txt, 16);
		text.setString(txt);
		text.setPosition(sf::Vector2f(850+256, 12 * i));
		display->draw(text);


		int value = loadFromAddress32((*r[SP]) + i * 4);
		_itoa_s(value, txt, 16);
		text.setString(txt);
		text.setPosition(sf::Vector2f(850+256 + 90, 12 * i));
		display->draw(text);
	}

	char msg[512];
	sprintf_s(msg, " PC: %02x\n R0: %02x\n R1: %02x\n R2: %02x\n R3: %02x\n R4: %02x\n R5: %02x\n R6: %02x\n R7: %02x\n R8: %02x\n FP(r11): %02x\n IP (r12): %02x\n SP: %02x\n LR: %02x\n CPSR: %02x\n Status: %02x\n, zero: %01x\n, carry: %01x\n, overflow: %01x\n, negative: %01x\n mode: %02x\n",
		*r[15], *r[0], *r[1], *r[2], *r[3], *r[4], *r[5], *r[6], *r[7], *r[8], *r[11], *r[12], *r[13], *r[14], cpsr.val, *r[16], cpsr.zero, cpsr.carry, cpsr.overflow, cpsr.negative, cpsr.mode);

	for (int i = 0; i < 1; i++){
		text.setString(msg);
		text.setPosition(sf::Vector2f(850+256, 130 + 12 * i));
		display->draw(text);
	}
	
	display->display();
}


void Display::handleEvents(){

	sf::Event event;
	while (display->pollEvent(event))
	{
		// "close requested" event: we close the window
		if (event.type == sf::Event::Closed)
			display->close();

		if (event.type == sf::Event::KeyPressed)
		{
			KEYINPUT.addr = loadFromAddress16(0x4000130, true);
			if (event.key.code == sf::Keyboard::Down)
			{
				KEYINPUT.btn_down = 0;
			}

			if (event.key.code == sf::Keyboard::Up)
			{
				KEYINPUT.btn_up = 0;
			}

			if (event.key.code == sf::Keyboard::Left)
			{
				KEYINPUT.btn_left = 0;
			}

			if (event.key.code == sf::Keyboard::Right)
			{
				KEYINPUT.btn_right = 0;
			}

			if (event.key.code == sf::Keyboard::Z)
			{
				KEYINPUT.btn_A = 0;
			}

			if (event.key.code == sf::Keyboard::X)
			{
				KEYINPUT.btn_B = 0;
			}
			if (event.key.code == sf::Keyboard::F)
			{
				debug = !debug;
			}
			if (event.key.code == sf::Keyboard::G)
			{
				if (debug)
					step = true;
			}
			writeToAddress16(0x4000130, KEYINPUT.addr);
		}

		else if (event.type == sf::Event::KeyReleased)
		{
			KEYINPUT.addr = loadFromAddress16(0x4000130, true);
			if (event.key.code == sf::Keyboard::Down)
			{
				KEYINPUT.btn_down = 1;
			}

			if (event.key.code == sf::Keyboard::Up)
			{
				KEYINPUT.btn_up = 1;
			}

			if (event.key.code == sf::Keyboard::Left)
			{
				KEYINPUT.btn_left = 1;
			}

			if (event.key.code == sf::Keyboard::Right)
			{
				KEYINPUT.btn_right = 1;
			}

			if (event.key.code == sf::Keyboard::Z)
			{
				KEYINPUT.btn_A = 1;
			}

			if (event.key.code == sf::Keyboard::X)
			{
				KEYINPUT.btn_B = 1;
			}
			writeToAddress16(0x4000130, KEYINPUT.addr);
		}
	}

}

void Display::updateStack(){

	//std::cout << *r[0] << " " << *r[1] << " " << *r[2] << " " << *r[3] << " " << *r[4] << " " << *r[5] << " " << *r[6] << " " << *r[7] << " FP (r11): " << *r[11] << " IP (r12): " << *r[12] << " SP: " << *r[13] << " LR: " << *r[14] << " CPRS: " << cprs << " SPRS " << cprs << "\n";

	display->display();
}


void Display::updateRegisterView(){



	display->display();

}