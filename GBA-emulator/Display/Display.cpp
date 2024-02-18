#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cstdio>
#include <iostream>

#include "Display/Display.h"
#include "Memory/MemoryOps.h"
#include "GBAcpu.h"
#include "Constants.h"
#include "Memory/memoryMappedIO.h"
#include "Gba-Graphics/Tile/Tile.h"
#include "Gba-Graphics/Tile/Tileset.h"
#include "Gba-Graphics/Rendermodes/TextMode.h"
#include "Gba-Graphics/Rendermodes/RenderMode3.h"
#include "Gba-Graphics/Sprites/Sprite.h"

extern RgbaPalette PaletteColours;
extern Tileset tileset;

Display::Display(int res_x, int res_y, std::string& name) : res_x(res_x), res_y(res_y), name(name){
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
	gameScreen.create(240, 160);
	gameScreen_s.setTexture(gameScreen);
	gameScreen_s.setPosition(0, 0);

	tmp3.create(256, 256 * 2);
	
	tilemapSprite.setTexture(tmp3, true);
	tilemapSprite.setPosition(257, 0);

	objMapTexture.create(256, 513);
	objMapSprite.setTexture(objMapTexture, true);
	objMapSprite.setPosition(257 * 3, 0);

	bgText[0].create(512, 512);
	bgText[1].create(512, 512);
	bgText[2].create(512, 512);
	bgText[3].create(512, 512);

	bgText[0].setRepeated(true);
	bgText[1].setRepeated(true);
	bgText[2].setRepeated(true);
	bgText[3].setRepeated(true);

	bgSprite[0].setTexture(bgText[0]);
	bgSprite[1].setTexture(bgText[1]);
	bgSprite[2].setTexture(bgText[2]);
	bgSprite[3].setTexture(bgText[3]);

	bgSprite[0].setPosition(512, 0);
	bgSprite[1].setPosition(512, 256);
	bgSprite[2].setPosition(512, 512);
	bgSprite[3].setPosition(512, 768);

	tileTxt.create(1024, 1024);

	gameTXT.create(512, 512);
	gameTXT.setRepeated(true);

	font.loadFromFile("GBA-emulator/arial.ttf");

	text.setFont(font);
	text.setCharacterSize(15);
	text.setStyle(sf::Text::Bold);
}

void Display::fillTiles(){
	if (!VRAMupdated){
		display->draw(tilemapSprite);
		return;  
	}
	BgCnt* bgCnt = (BgCnt*)&IoRAM[8];
	
	tileset.update();
	tmp3.update(tileset.getTileset(bgCnt->is8Bit));

	display->draw(tilemapSprite);
}

void Display::fillBG(uint32_t regOffset){
	//if (!VRAMupdated)
	//return;
	
	BgCnt* bgCnt = (BgCnt*)&IoRAM[8 + regOffset];
	uint32_t startAddr = 0;
	constexpr uint32_t scalar = 255 / 31;

	uint16_t size_x = bgCnt->hWide ? 512 : 256;
	uint16_t size_y = bgCnt->vWide ? 512 : 256;

	if (displayCtrl->bgMode == 0 || displayCtrl->bgMode == 1 || displayCtrl->bgMode == 2){
		textMode.draw(regOffset);
		bgSprite[(regOffset / 2)].setTextureRect(sf::IntRect(0, 0, 256, 256));
		bgText[(regOffset / 2)].update((uint8_t*)textMode.getBG());
	}

	if (displayCtrl->bgMode == 3){
		renderMode3.draw();
		bgSprite[(regOffset / 2)].setTextureRect(sf::IntRect(0, 0, 240, 160));
		bgText[(regOffset / 2)].update((uint8_t*)renderMode3.getBG());
	}

	else if (displayCtrl->bgMode == 4){
		renderMode4.draw();
		bgSprite[(regOffset / 2)].setTextureRect(sf::IntRect(0, 0, 240, 160));
		bgText[(regOffset / 2)].update((uint8_t*)renderMode4.getBG());
	}

	else if (displayCtrl->bgMode == 5) {
		renderMode5.draw();
		bgSprite[(regOffset / 2)].setTextureRect(sf::IntRect(0, 0, 240, 160));
		bgText[(regOffset / 2)].update((uint8_t*)renderMode5.getBG());
	}
	

	bgSprite[(regOffset / 2)].setScale(256.0f / size_x, 256.0f / size_y);
	display->draw(bgSprite[(regOffset / 2)]);
	VRAMupdated = false;
}

void Display::fillObjects(uint32_t regOffset){
	if (!OBJupdated){
		display->draw(objMapSprite);
		return;
	}

	BgCnt* bgCnt = (BgCnt*)&IoRAM[8 + regOffset];
	spriteObjects.update();
	objMapTexture.update((uint8_t*)spriteObjects.getTileset(bgCnt->is8Bit), 256, 256, 0, 0);

	display->draw(objMapSprite);
	OBJupdated = false;
}



void Display::appendBGs() {
	
	BgCnt* bgCnt0 = (BgCnt*)&IoRAM[0x8];
	BgCnt* bgCnt1 = (BgCnt*)&IoRAM[0xA];
	BgCnt* bgCnt2 = (BgCnt*)&IoRAM[0xC];
	BgCnt* bgCnt3 = (BgCnt*)&IoRAM[0xE];
	gameTXT.clear(sf::Color(0, 0, 0));

	std::vector<uint8_t> bgOrder[4];
	memset(finalImage.finalImagePalette.data(), 0xFFFF'FFFF, sizeof(finalImage.finalImagePalette));

	constexpr uint16_t screenSizeX = 240;
	constexpr uint16_t screenSizey = 160;

	const bool isBitmapMode = (displayCtrl->bgMode == 3);

	spriteObjects.updateSpritePriorities();

	if (displayCtrl->bg3Display) {
			bgOrder[bgCnt3->priority].push_back(6);
	}
	if (displayCtrl->bg2Display) {
			bgOrder[bgCnt2->priority].push_back(4);
	}
	if (displayCtrl->bg1Display) {
			bgOrder[bgCnt1->priority].push_back(2);
	}
	if (displayCtrl->bg0Display) {
			bgOrder[bgCnt0->priority].push_back(0);
	}

	if (displayCtrl->bgMode == 3) {
		renderMode3.fillToDisplay(finalImage.finalImageColors);
	}

	if (displayCtrl->bgMode == 4) {
		renderMode4.fillImage(finalImage.finalImagePalette);
	}

	for (int vec = 3; vec >= 0; vec--) {
		for (auto& offset : bgOrder[vec]) {
			if (displayCtrl->bgMode == 0) {
				textMode.draw(offset);
				textMode.fillImage(finalImage.finalImagePalette, offset);
			}
		}
	}

	for (int tst = 126; tst >= 0; tst--) {
		auto s1 = Sprite(spriteObjects, tst * 8);
		s1.update();
		s1.fillToImg(PaletteColours, finalImage.finalImagePalette, isBitmapMode);
	}

	if(!isBitmapMode)
		realizePalettes(PaletteColours, finalImage);

	gameScreen.update(reinterpret_cast<uint8_t*>(&finalImage.finalImageColors));
	
	gameTXT.draw(gameScreen_s);
	gameTXT.display();

	sf::Sprite img = sf::Sprite(gameTXT.getTexture(), sf::IntRect(0, 0, 240, 160));
	img.setPosition(0, 512);
	img.scale(2, 2);
	display->draw(img);
}

void Display::realizePalettes(const RgbaPalette& palette, FinalImage& finalImage)
{
	for(size_t y = 0; y < 160; y++)
		for (size_t x = 0; x < 240; x++) {
			auto bitmapInfo = finalImage.finalImagePalette[y][x];
			if (bitmapInfo.index == 0xFFFF) {
				bitmapInfo.index = 0;
				bitmapInfo.palette = 0;
			}
			auto clr = palette.colorFromIndex(bitmapInfo.palette, bitmapInfo.index);
			finalImage.finalImageColors[y][x].rawColor = clr.rawColor;
		}
}

void Display::updatePalettes(){
	float fps;
	sf::Clock clock = sf::Clock::Clock();
	sf::Time previousTime = clock.getElapsedTime();
	sf::Time currentTime;

	display->clear(sf::Color::Black);

	fillObjects(0);
	fillTiles();
	PaletteColours.updatePalette();
	
	if (displayCtrl->bg0Display) {
		fillBG(0);
	}
	if (displayCtrl->bg1Display) {
		fillBG(2);
	}
	if (displayCtrl->bg2Display) {
		fillBG(4);
	}
	if (displayCtrl->bg3Display) {
		fillBG(6);
	}
	
	appendBGs();

	for (int i = 0; i < 10; i++){
		char txt[16];
		_itoa_s((r[SP]) + i * 4, txt, 16);
		text.setString(txt);
		text.setPosition(sf::Vector2f(850 + 256, 12 * i));
		display->draw(text);


		int value = loadFromAddress32((r[SP]) + i * 4, true);
		_itoa_s(value, txt, 16);
		text.setString(txt);
		text.setPosition(sf::Vector2f(850 + 256 + 90, 12 * i));
		display->draw(text);
	}

	char msg[512];
	sprintf_s(msg, " PC: %02x\n R0: %02x\n R1: %02x\n R2: %02x\n R3: %02x\n R4: %02x\n R5: %02x\n R6: %02x\n R7: %02x\n R8: %02x\n FP(r11): %02x\n IP (r12): %02x\n SP: %02x\n LR: %02x\n CPSR: %02x\n Status: %02x\n, zero: %01x\n, carry: %01x\n, overflow: %01x\n, negative: %01x\n mode: %02x\n",
		r[15], r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], r[8], r[11], r[12], r[13], r[14], cpsr.val, r[16], cpsr.zero, cpsr.carry, cpsr.overflow, cpsr.negative, cpsr.mode);

	text.setString(msg);
	text.setPosition(sf::Vector2f(850 + 256, 130 + 12));
	display->draw(text);

	currentTime = clock.getElapsedTime();
	fps = 1.0f / (currentTime.asSeconds() - previousTime.asSeconds()); // the asSeconds returns a float
	
	sprintf_s(msg, "FPS %f ", fps);
	text.setString(msg);
	text.setPosition(sf::Vector2f(1100, 530));
	display->draw(text);
	
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
			if (event.key.code == sf::Keyboard::Down)
			{
				keyInput->btn_down = 0;
			}

			if (event.key.code == sf::Keyboard::Up)
			{
				keyInput->btn_up = 0;
			}

			if (event.key.code == sf::Keyboard::Left)
			{
				keyInput->btn_left = 0;
			}

			if (event.key.code == sf::Keyboard::Right)
			{
				keyInput->btn_right = 0;
			}

			if (event.key.code == sf::Keyboard::Z)
			{
				keyInput->btn_A = 0;
			}

			if (event.key.code == sf::Keyboard::X)
			{
				keyInput->btn_B = 0;
			}

			if (event.key.code == sf::Keyboard::A)
			{
				keyInput->btn_l = 0;
			}

			if (event.key.code == sf::Keyboard::S)
			{
				keyInput->btn_r = 0;
			}

			if (event.key.code == sf::Keyboard::Q)
			{
				keyInput->btn_start = 0;
			}

			if (event.key.code == sf::Keyboard::W)
			{
				keyInput->btn_select = 0;
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
			/*
			if (keypadInterruptCtrl->IRQ_EN && InterruptEnableRegister->keyPad){
				uint16_t tmp = ~((keyInput->addr) & 0x3FFF);
				if (keypadInterruptCtrl->IRQ_cond){
					if (tmp & (keypadInterruptCtrl->addr & 0x3FFF)){
						InterruptFlagRegister->keyPad = 1;
					}
				}
				else{
					if ((tmp & (keyInput->addr & 0x3FFF)) == tmp){
						InterruptFlagRegister->keyPad = 1;
					}
				}
			}
			*/
		}

		else if (event.type == sf::Event::KeyReleased)
		{
			if (event.key.code == sf::Keyboard::Down)
			{
				keyInput->btn_down = 1;
			}

			if (event.key.code == sf::Keyboard::Up)
			{
				keyInput->btn_up = 1;
			}

			if (event.key.code == sf::Keyboard::Left)
			{
				keyInput->btn_left = 1;
			}

			if (event.key.code == sf::Keyboard::Right)
			{
				keyInput->btn_right = 1;
			}

			if (event.key.code == sf::Keyboard::Z)
			{
				keyInput->btn_A = 1;
			}

			if (event.key.code == sf::Keyboard::X)
			{
				keyInput->btn_B = 1;
			}

			if (event.key.code == sf::Keyboard::A)
			{
				keyInput->btn_l = 1;
			}

			if (event.key.code == sf::Keyboard::S)
			{
				keyInput->btn_r = 1;
			}

			if (event.key.code == sf::Keyboard::Q)
			{
				keyInput->btn_start = 1;
			}

			if (event.key.code == sf::Keyboard::W)
			{
				keyInput->btn_select = 1;
			}
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