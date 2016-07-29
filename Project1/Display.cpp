#include "Display.h"
#include "MemoryOps.h"
#include <SFML/Graphics.hpp>

using namespace sf;

Display::Display(int res_x, int res_y, char* name) : res_x(res_x), res_y(res_y), name(name){
	display = new RenderWindow(VideoMode(res_x, res_y), name);
}

void Display::updatePalettes(){
	//static int asd = 0;
	display->clear(Color::Black);
	scanPalettes();
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
	//for bg palettes
	for (int i = 0; i < 16; i++){
		for (int k = 0; k < 16; k++){
			int setting = loadFromAddress16(startAddr);
			int red = setting & 0x1F;
			int green = (setting >> 5) & 0x1F;
			int blue = (setting >> 10) & 0x1F;
			int redScaled = red * 255 / 31;
			int greenScaled = green * 255 / 31;
			int blueScaled = blue * 255 / 31;
			Color color(redScaled, greenScaled, blueScaled);
			rectangle.setFillColor(color);
			rectangle.setPosition(Vector2f(16*k, 16*i));
			display->draw(rectangle);
			startAddr += 2;
		}
	}

	//for fg palettes
	for (int i = 0; i < 16; i++){
		for (int k = 0; k < 16; k++){
			int setting = loadFromAddress16(startAddr);
			int red = setting & 0x1F;
			int green = (setting >> 5) & 0x1F;
			int blue = (setting >> 10) & 0x1F;
			int redScaled = red * 255 / 31;
			int greenScaled = green * 255 / 31;
			int blueScaled = blue * 255 / 31;
			Color color(redScaled, greenScaled, blueScaled);
			rectangle.setFillColor(color);
			rectangle.setPosition(Vector2f(16 * k, 272 + 16 * i));
			display->draw(rectangle);
			startAddr += 2;
		}
	}
	
}

