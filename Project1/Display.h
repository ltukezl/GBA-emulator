#ifndef DISPLAY_H
#define DISPLAY_H
#include <SFML/Graphics.hpp>
#include <stdint.h>


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
};

extern Display* debugView;

#endif