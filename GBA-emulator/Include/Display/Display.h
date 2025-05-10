#pragma once
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <cstring>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

#include "Constants.h"
#include "Memory/memoryMappedIO.h"
#include "Gba-Graphics/Tile/Tileset.h"
#include "Gba-Graphics/Rendermodes/TextMode.h"
#include "Gba-Graphics/Rendermodes/RenderMode3.h"
#include "Gba-Graphics/Rendermodes/RenderMode4.h"
#include "Gba-Graphics/Rendermodes/RenderMode5.h"
#include "Gba-Graphics/sprites/SpriteGenerator.h"

struct Ring{
	char state[512];
	Ring* next;
};

class Display{
	size_t res_x, res_y;
	sf::RenderWindow* display;
	std::string name;

	sf::Sprite tilemapSprite;

	sf::Texture tileMap1Texture;

	sf::Texture objMapTexture;
	sf::Sprite objMapSprite;

	sf::Texture bgText[4];

	sf::RenderTexture tileTxt;
	sf::RenderTexture gameTXT;

	sf::Sprite bgSprite[4];

	sf::Texture tmp3;
	sf::Texture gameScreen;
	sf::Sprite gameScreen_s;

	sf::Font font;
	sf::Text text;

	SpriteGenerator spriteObjects;

	TextMode textMode;
	RenderMode3 renderMode3;
	RenderMode4 renderMode4;
	RenderMode5 renderMode5;

	union FinalImage{
		finalImagePalettes finalImagePalette;
		finalImageColored finalImageColors;
	} finalImage;


public:

	struct OamSize{ uint8_t x; uint8_t y; };

	bool VRAMupdated = false;
	bool OBJupdated = false;

	Display(int, int, std::string&);

	void updatePalettes();

	void handleEvents();

	void updateStack();

	void updateRegisterView();

	void fillBG(uint32_t regOffset);

	void fillTiles();

	void fillObjects(uint32_t regOffset);

	void appendBGs();

	void realizePalettes(const RgbaPalette& palette, FinalImage& finalImage);

private:
	int cnt;
	Ring* txtRing;
};

extern Display* debugView;