#ifndef DISPLAY_H
#define DISPLAY_H
#include <SFML/Graphics.hpp>

using namespace sf;

class Display{
	int res_x, res_y;
	RenderWindow* display;
	char* name;
public:
	Display(int, int, char*);

	void updatePalettes();

	void scanPalettes();

	void handleEvents();

	void updateStack();
};



#endif