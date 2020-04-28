#ifndef DISPLAY_H
#define DISPLAY_H
#include <SFML/Graphics.hpp>


struct Ring{
	char state[512];
	Ring* next;
};

class Display{
	int res_x, res_y;
	sf::RenderWindow* display;
	char* name;

	sf::Color bgPaletteColors[256];
	sf::Image tileMap1[32 * 32];

public:
	Display(int, int, char*);

	void updatePalettes();

	void scanPalettes();

	void handleEvents();

	void updateStack();

	void updateRegisterView();
private:
	int cnt;
	Ring* txtRing;
};



#endif