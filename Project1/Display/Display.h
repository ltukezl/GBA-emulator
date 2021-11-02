#ifndef DISPLAY_H
#define DISPLAY_H
#include <SFML/Graphics.hpp>
#include <stdint.h>
#include "Memory/memoryMappedIO.h"


struct Ring{
	char state[512];
	Ring* next;
};

class Display{
	int res_x, res_y;
	sf::RenderWindow* display;
	char* name;

	sf::Sprite paletteSprite;
	sf::Color PaletteColors[256 * 2];
	sf::Image tileMap[32 * 32 * 2];
	sf::Image objMap[32 * 32 * 2];
	uint8_t colors[2 * 16 * 16 * 4];
	uint8_t bgTile[8 * 8 * 4];

	sf::Texture paletteTexture;
	sf::Image paletteTile;

	sf::Image tmpTile;
	sf::Sprite tilemapSprite;

	sf::Texture tileMap1Texture;

	sf::Texture objMapTexture;
	sf::Sprite objMapSprite;

	sf::Texture bgText[4];

	sf::RenderTexture gameTXT;

	

public:

	struct OamSize{ uint8_t x; uint8_t y; };

	bool VRAMupdated = false;
	bool OBJupdated = false;

	Display(int, int, char*);

	void updatePalettes();

	void scanPalettes();

	void handleEvents();

	void updateStack();

	void updateRegisterView();

	void fillBG(uint32_t regOffset);

	void fillTiles(uint32_t regOffset);

	void fillObjects(uint32_t regOffset);

	void appendBGs();

private:
	int cnt;
	Ring* txtRing;

	sf::Image& calculate4BitTile(uint32_t base, BgTile* tile);
	sf::Image& calculate8BitTile(uint32_t base, BgTile* tile);
};

extern Display* debugView;

#endif