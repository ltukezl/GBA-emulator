#ifndef DISPLAY_H
#define DISPLAY_H
#include <SFML/Graphics.hpp>
#include <stdint.h>
#include <string.h>
#include "Memory/memoryMappedIO.h"


struct Ring{
	char state[512];
	Ring* next;
};

class Display{
	int res_x, res_y;
	sf::RenderWindow* display;
	std::string name;

	sf::Sprite paletteSprite;
	sf::Color PaletteColors[256 * 2];
	sf::Image tileMap[32 * 32 * 2];
	sf::Image objMap[32 * 32 * 2];
	uint8_t colors[2 * 16 * 16 * 4] = { 0 };
	uint8_t localColors[8 * 8 * 4] = { 0 };
	uint8_t localColors2[512][256][4] = { 0 };
	uint8_t localColors3[512][512][4] = { 0 };

	sf::Texture paletteTexture;
	sf::Image paletteTile;

	sf::Image tmpTile;
	sf::Sprite tilemapSprite;

	sf::Texture tileMap1Texture;

	sf::Texture objMapTexture;
	sf::Sprite objMapSprite;

	sf::Texture bgText[4];

	sf::RenderTexture tileTxt;
	sf::RenderTexture gameTXT;

	sf::Sprite bgSprite[4];

	sf::Texture tmp3;

public:

	struct OamSize{ uint8_t x; uint8_t y; };

	bool VRAMupdated = false;
	bool OBJupdated = false;

	Display(int, int, std::string&);

	void updatePalettes();

	void scanPalettes();

	void handleEvents();

	void updateStack();

	void updateRegisterView();

	void fillBG(uint32_t regOffset);

	void fillTiles();

	void fillObjects(uint32_t regOffset);

	void appendBGs();

private:
	int cnt;
	Ring* txtRing;

	void calculate4BitTile(uint32_t y, uint32_t x, uint32_t base, BgTile* tile);
	void calculate8BitTile(uint32_t y, uint32_t x, uint32_t base, BgTile* tile);
};

extern Display* debugView;

#endif