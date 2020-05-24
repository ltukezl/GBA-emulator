#include <SFML/Graphics.hpp>
#include <stdio.h>
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

void Display::updatePalettes(){
	display->clear(sf::Color::Black);
	scanPalettes();

	uint16_t bg1Control = IoRAM[8];

	int startAddr = 0x06000000;

	sf::Image tile;
	tile.create(8, 8, sf::Color::Blue);
	for (int tileY = 0; tileY < 32; tileY++)
	for (int tileX = 0; tileX < 32; tileX++){
		for (int y = 0; y < 8; y++){
			int row = loadFromAddress32(startAddr, true);
			for (int pixel = 0; pixel < 8; pixel++){
				int color = (row & 0xf);
				tile.setPixel(pixel, y, bgPaletteColors[color]);
				row >>= 4;
			}
			startAddr += 4;
		}
		tileMap1[32 * tileY + tileX] = tile;
	}


	sf::Texture tileMap1Texture;
	tileMap1Texture.create(256, 513);

	for (int i = 0; i < 32; i++)
		for (int k = 0; k < 32; k++){
			sf::Image tile = tileMap1[32 * i + k];
			tileMap1Texture.update(tile, 8 * k, 8 * i);
		}

	sf::Sprite tilemapSprite;
	tilemapSprite.setTexture(tileMap1Texture, true);
	tilemapSprite.setPosition(257, 0);

	display->draw(tilemapSprite);


	startAddr = 0x06000000;

	sf::Texture BG1Texture;
	BG1Texture.create(256, 256);
	for (int i = 0; i < 32; i++)
		for (int k = 0; k < 32; k++){
			uint16_t reg = loadFromAddress16(startAddr, true);
			uint16_t tilNum = reg & 0x1FF;
			BG1Texture.update(tileMap1[0x200 + tilNum], 8 * k, 8 * i);
			startAddr += 2;
		}

	sf::Sprite BG1Sprite;
	BG1Sprite.setTexture(BG1Texture, true);
	BG1Sprite.setPosition(514, 0);

	display->draw(BG1Sprite);


	/*
	sf::Font font;
	font.loadFromFile("arial.ttf");

	sf::Text text;
	text.setFont(font);
	text.setColor(sf::Color::Red);
	text.setCharacterSize(10);
	text.setStyle(sf::Text::Bold);

	for (int i = 0; i < 10; i++){
		char txt[16];
		_itoa_s((*r[SP]) + i * 4, txt, 16);
		text.setString(txt);
		text.setPosition(sf::Vector2f(260, 10 * i));
		display->draw(text);


		int value = loadFromAddress32((*r[SP]) + i * 4);
		_itoa_s(value, txt, 16);
		text.setString(txt);
		text.setPosition(sf::Vector2f(260 + 60, 10 * i));
		display->draw(text);
	}

	char msg[512];
	sprintf_s(msg, " PC: %02x\n R0: %02x\n R1: %02x\n R2: %02x\n R3: %02x\n R4: %02x\n R5: %02x\n R6: %02x\n R7: %02x\n FP(r11): %02x\n IP (r12): %02x\n SP: %02x\n LR: %02x\n CPSR: %02x\n Status: %02x",
		*r[15], *r[0], *r[1], *r[2], *r[3], *r[4], *r[5], *r[6], *r[7], *r[11], *r[12], *r[13], *r[14], cprs, *r[16]);

	for (int i = 0; i < 1; i++){
		text.setString(msg);
		text.setPosition(sf::Vector2f(260, 110 + 10 * i));
		display->draw(text);
	}
	*/
	display->display();
}


void Display::handleEvents(){

	sf::Event event;
	while (display->pollEvent(event))
	{
		// "close requested" event: we close the window
		if (event.type == sf::Event::Closed)
			display->close();
	}

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
			bgPaletteColors[16 * i + k] = color;
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
			rectangle.setFillColor(color);
			rectangle.setPosition(sf::Vector2f(16 * k, 272 + 16 * i));
			display->draw(rectangle);
			startAddr += 2;
	}
}

void Display::updateStack(){

	//std::cout << *r[0] << " " << *r[1] << " " << *r[2] << " " << *r[3] << " " << *r[4] << " " << *r[5] << " " << *r[6] << " " << *r[7] << " FP (r11): " << *r[11] << " IP (r12): " << *r[12] << " SP: " << *r[13] << " LR: " << *r[14] << " CPRS: " << cprs << " SPRS " << *r[16] << "\n";

	display->display();
}


void Display::updateRegisterView(){



	display->display();

}