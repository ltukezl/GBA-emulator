#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <iostream>
#include "Display.h"
#include "Memory/MemoryOps.h"
#include "GBAcpu.h"
#include "Constants.h"
#include "Memory/memoryMappedIO.h"

struct Display::OamSize shapes[3][4] = { { { 8, 8 }, { 16, 16 }, { 32, 32 }, { 64, 64 } },
										{ { 16, 8 }, { 32, 8 }, { 32, 16 }, { 64, 32 } },
										{ { 8, 16 }, { 8, 32 }, { 16, 32 }, { 32, 64 } } };

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

	paletteTexture.create(16, 32);
	paletteTile.create(16, 32, sf::Color::Black);

	paletteSprite.setTexture(paletteTexture, true);
	paletteSprite.setPosition(0, 0);
	paletteSprite.setScale(16.0, 16.0);

	tmpTile.create(8, 8);
	tmpTile.createMaskFromColor(sf::Color(0, 0, 0));
	tileMap1Texture.create(256, 513);

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
}

void Display::calculate4BitTile(uint32_t y, uint32_t x, uint32_t base, BgTile* tile){
	uint32_t pixeloffset = 0;
	for (int pixelY = 0; pixelY < 8; pixelY++) {
		int row = loadFromAddress32(0x6000000 + base + tile->tileNumber * 0x20 + pixeloffset, true);
		for (int pixelX = 0; pixelX < 8; pixelX++) {
			int color = (row & 0xf);
			localColors3[y * 8 + pixelY][x * 8 + pixelX][0] = PaletteColors[16 * tile->paletteNum + color].r;
			localColors3[y * 8 + pixelY][x * 8 + pixelX][1] = PaletteColors[16 * tile->paletteNum + color].g;
			localColors3[y * 8 + pixelY][x * 8 + pixelX][2] = PaletteColors[16 * tile->paletteNum + color].b;
			localColors3[y * 8 + pixelY][x * 8 + pixelX][3] = PaletteColors[16 * tile->paletteNum + color].a;
			row >>= 4;
		}
		pixeloffset += 4;
	}
}

void Display::calculate8BitTile(uint32_t y, uint32_t x, uint32_t base, BgTile* tile){
	uint32_t offset = 0x6000000 + base + tile->tileNumber * 0x40;
	for (int pixelY = 0; pixelY < 8; pixelY++) {
		for (int pixelX = 0; pixelX < 8; pixelX++) {
			int color = loadFromAddress(offset, true);
			localColors3[y * 8 + pixelY][x * 8 + pixelX][0] = PaletteColors[color].r;
			localColors3[y * 8 + pixelY][x * 8 + pixelX][1] = PaletteColors[color].g;
			localColors3[y * 8 + pixelY][x * 8 + pixelX][2] = PaletteColors[color].b;
			localColors3[y * 8 + pixelY][x * 8 + pixelX][3] = PaletteColors[color].a;
			offset++;
		}
	}
}

void Display::scanPalettes() {
	int startAddr = 0;
	int colorPtr = 0;
	const int scalar = 255 / 31;

	//for bg palettes
	for (int i = 0; i < 32; i++)
		for (int k = 0; k < 16; k++) {
			ColorPaletteRam* colorPaletteRam = (ColorPaletteRam*)&PaletteRAM[startAddr];
			int redScaled = colorPaletteRam->red * scalar;
			int greenScaled = colorPaletteRam->green * scalar;
			int blueScaled = colorPaletteRam->blue * scalar;
			sf::Color color(redScaled, greenScaled, blueScaled);
			//if (k == 0)
				//color.a = 0;
			PaletteColors[16 * i + k] = color;
			colors[colorPtr + 0] = color.r;
			colors[colorPtr + 1] = color.g;
			colors[colorPtr + 2] = color.b;
			colors[colorPtr + 3] = color.a;
			startAddr += 2;
			colorPtr += 4;
		}

	paletteTexture.update(colors);
	display->draw(paletteSprite);
}

void Display::fillTiles(){
	if (!VRAMupdated){
		display->draw(tilemapSprite);
		return;
	}

	int startAddr = 0;
	for (int tileY = 0; tileY < 64; tileY++){
		for (int tileX = 0; tileX < 32; tileX++) {
			for (int y = 0; y < 8; y++) {
				int row = rawLoad32(VRAM, startAddr);
				for (int pixel = 0; pixel < 8; pixel++) {
					int color = (row & 0xf);
					auto clr = PaletteColors[color];
					localColors2[tileY * 8 + y][tileX * 8 + pixel][0] = clr.r;
					localColors2[tileY * 8 + y][tileX * 8 + pixel][1] = clr.g;
					localColors2[tileY * 8 + y][tileX * 8 + pixel][2] = clr.b;
					localColors2[tileY * 8 + y][tileX * 8 + pixel][3] = clr.a;
					row >>= 4;
				}
				startAddr += 4;
			}
		}
	}

	tmp3.update((uint8_t*)localColors2);
	display->draw(tilemapSprite);
}

void Display::fillBG(uint32_t regOffset){
	//if (!VRAMupdated)
	//return;

	BgCnt* bgCnt = (BgCnt*)&IoRAM[8 + regOffset];
	uint32_t startAddr = 0;
	uint32_t tileBaseBlock = 0;
	const int scalar = 255 / 31;

	/*fills BG map*/
	uint16_t size_x = bgCnt->hWide ? 512 : 256;
	uint16_t size_y = bgCnt->vWide ? 512 : 256;

	if (displayCtrl->bgMode == 0 || displayCtrl->bgMode == 1 || displayCtrl->bgMode == 2){
		uint32_t startAddr = bgCnt->bgBaseblock * 0x800;
		uint32_t tileBaseBlock = bgCnt->tileBaseBlock * 0x4000;
		for (int i = 0; i < 32; i++){
			for (int k = 0; k < 32; k++){
				BgTile* tile = (BgTile*)&VRAM[startAddr];

				if (bgCnt->palettes == 0)
					calculate4BitTile(i, k, tileBaseBlock, tile);
				else
					calculate8BitTile(i, k, tileBaseBlock, tile);

				startAddr += 2;
			}
		}

		if (bgCnt->hWide){
			for (int i = 0; i < 32; i++){
				for (int k = 0; k < 32; k++){
					BgTile* tile = (BgTile*)&VRAM[startAddr];

					if (bgCnt->palettes == 0)
						calculate4BitTile(i, 32 + k, tileBaseBlock, tile);
					else
						calculate8BitTile(i, 32 + k, tileBaseBlock, tile);

					startAddr += 2;
				}
			}
		}
		if (bgCnt->vWide){
			for (int i = 0; i < 32; i++){
				for (int k = 0; k < 32; k++){
					BgTile* tile = (BgTile*)&VRAM[startAddr];

					if (bgCnt->palettes == 0)
						calculate4BitTile(32 + i, k, tileBaseBlock, tile);
					else
						calculate8BitTile(32 + i, k, tileBaseBlock, tile);

					startAddr += 2;
				}
			}
		}
		if (bgCnt->vWide && bgCnt->hWide){
			for (int i = 0; i < 32; i++){
				for (int k = 0; k < 32; k++){
					BgTile* tile = (BgTile*)&VRAM[startAddr];

					if (bgCnt->palettes == 0)
						calculate4BitTile(32 + i, 32 + k, tileBaseBlock, tile);
					else
						calculate8BitTile(32 + i, 32 + k, tileBaseBlock, tile);

					startAddr += 2;
				}
			}
		}

		bgSprite[(regOffset / 2)].setTextureRect(sf::IntRect(0, 0, size_x, size_y));
		display->draw(bgSprite[(regOffset / 2)]);
	}

	else if (displayCtrl->bgMode == 3){
		for (int k = 0; k < 160; k++)
			for (int i = 0; i < 240; i++){
				ColorPaletteRam* colorPaletteRam = (ColorPaletteRam*)&VRAM[startAddr];
				int redScaled = colorPaletteRam->red * scalar;
				int greenScaled = colorPaletteRam->green * scalar;
				int blueScaled = colorPaletteRam->blue * scalar;
				localColors3[k][i][0] = redScaled;
				localColors3[k][i][1] = greenScaled;
				localColors3[k][i][2] = blueScaled;
				localColors3[k][i][3] = 255;
				startAddr += 2;
			}
		sf::Sprite BG1Sprite;
		
		bgSprite[(regOffset / 2)].setTextureRect(sf::IntRect(0, 0, 240, 160));
		
	}
	else if (displayCtrl->bgMode == 4){
		sf::Image tile;
		for (int k = 0; k < 160; k++)
			for (int i = 0; i < 240; i++){
				uint8_t colorIdx = loadFromAddress(0x6000000 + startAddr++);
				localColors3[k][i][0] = PaletteColors[colorIdx].r;
				localColors3[k][i][1] = PaletteColors[colorIdx].g;
				localColors3[k][i][2] = PaletteColors[colorIdx].b;
				if(colorIdx != 0)
					localColors3[k][i][3] = 255;
				else
					localColors3[k][i][3] = 0;
			}
		bgSprite[(regOffset / 2)].setTextureRect(sf::IntRect(0, 0, 240, 160));
	}

	bgText[(regOffset / 2)].update((uint8_t*)localColors3);
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
	int startAddr = 0x06010000;

	/*creates sprites 1 and 2*/
	for (int tileY = 0; tileY < 32; tileY++)
		for (int tileX = 0; tileX < 32; tileX++){
			for (int pixelY = 0; pixelY < 8; pixelY++) {
				int row = loadFromAddress32(startAddr, true);
				for (int pixelX = 0; pixelX < 8; pixelX++) {
					int color = (row & 0xf);
					localColors2[tileY * 8 + pixelY][tileX * 8 + pixelX][0] = PaletteColors[256 + color].r;
					localColors2[tileY * 8 + pixelY][tileX * 8 + pixelX][1] = PaletteColors[256 + color].g;
					localColors2[tileY * 8 + pixelY][tileX * 8 + pixelX][2] = PaletteColors[256 + color].b;
					localColors2[tileY * 8 + pixelY][tileX * 8 + pixelX][3] = PaletteColors[256 + color].a;
					row >>= 4;
				}
				startAddr += 4;
			}
		}
	objMapTexture.update((uint8_t*)localColors2, 256, 256, 0, 0);

	/*draws sprites 1 and 2 to screen*/
	display->draw(objMapSprite);
	OBJupdated = false;
}

void Display::appendBGs(){
	BgCnt* bgCnt0 = (BgCnt*)&IoRAM[0x8];
	BgCnt* bgCnt1 = (BgCnt*)&IoRAM[0xA];
	BgCnt* bgCnt2 = (BgCnt*)&IoRAM[0xC];
	BgCnt* bgCnt3 = (BgCnt*)&IoRAM[0xE];
	gameTXT.clear(sf::Color(0, 0, 0));

	std::vector<sf::Sprite> renderOrder[4];

	uint16_t screenSizeX = 0;
	uint16_t screenSizey = 0;

	if (displayCtrl->bgMode == 0){
		screenSizeX = 256;
		screenSizey = 256;
	}
	else if (displayCtrl->bgMode == 3 || displayCtrl->bgMode == 4){
		screenSizeX = 240;
		screenSizey = 160;
	}

	if (displayCtrl->bg3Display){
		if (displayCtrl->bgMode == 0)
			renderOrder[bgCnt3->priority].push_back(sf::Sprite(bgText[3], sf::IntRect(BG3HOFS->offset, BG3VOFS->offset, screenSizeX, screenSizey)));
		else
			renderOrder[bgCnt3->priority].push_back(sf::Sprite(bgText[3]));
	}
	if (displayCtrl->bg2Display){
		if (displayCtrl->bgMode == 0)
			renderOrder[bgCnt2->priority].push_back(sf::Sprite(bgText[2], sf::IntRect(BG2HOFS->offset, BG2VOFS->offset, screenSizeX, screenSizey)));
		else
			renderOrder[bgCnt2->priority].push_back(sf::Sprite(bgText[2]));
	}
	if (displayCtrl->bg1Display){
		if (displayCtrl->bgMode == 0 || displayCtrl->bgMode == 1)
			renderOrder[bgCnt1->priority].push_back(sf::Sprite(bgText[1], sf::IntRect(BG1HOFS->offset, BG1VOFS->offset, screenSizeX, screenSizey)));
		else
			renderOrder[bgCnt1->priority].push_back(sf::Sprite(bgText[1]));
	}
	if (displayCtrl->bg0Display){
		if (displayCtrl->bgMode == 0 || displayCtrl->bgMode == 1){
			auto s = sf::Sprite(bgText[0], sf::IntRect(BG0HOFS->offset, BG0VOFS->offset, screenSizeX, screenSizey));
			renderOrder[bgCnt0->priority].push_back(s);
		}	
		else
			renderOrder[bgCnt0->priority].push_back(sf::Sprite(bgText[0]));
	}

	if (displayCtrl->objDisplay){
		uint16_t startAddr = 0x3f8;
		for (int object = 127; object >= 0; object--){
			ObjReg1* objr1 = (ObjReg1*)&OAM[startAddr + 0];
			ObjReg2* objr2 = (ObjReg2*)&OAM[startAddr + 4];
			startAddr -= 8;
			if (objr1->isDoubleOrNoDisplay == 0){
				sf::Sprite s(objMapTexture, sf::IntRect((objr2->tileNumber % 32) * 8, (objr2->tileNumber / 32) * 8, shapes[objr1->shape][objr1->size].x, shapes[objr1->shape][objr1->size].y));
				s.setPosition(objr1->xCoord, objr1->yCoord);

				renderOrder[objr2->priority].push_back(s);
			}
		}
	}
	
	for (int vec = 3; vec >= 0; vec--){
		for(auto& s: renderOrder[vec])
			gameTXT.draw(s);
	}

	gameTXT.display();

	sf::Sprite img = sf::Sprite(gameTXT.getTexture(), sf::IntRect(0, 0, 240, 160));
	img.setPosition(0, 512);
	img.scale(2, 2);
	display->draw(img);
}

void Display::updatePalettes(){
	display->clear(sf::Color::Black);

	scanPalettes();
	fillObjects(0);
	fillTiles();

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

	sf::Font font;
	font.loadFromFile("Project1/arial.ttf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(15);
	text.setStyle(sf::Text::Bold);

	for (int i = 0; i < 10; i++){
		char txt[16];
		_itoa_s((*r[SP]) + i * 4, txt, 16);
		text.setString(txt);
		text.setPosition(sf::Vector2f(850 + 256, 12 * i));
		display->draw(text);


		int value = loadFromAddress32((*r[SP]) + i * 4, true);
		_itoa_s(value, txt, 16);
		text.setString(txt);
		text.setPosition(sf::Vector2f(850 + 256 + 90, 12 * i));
		display->draw(text);
	}

	char msg[512];
	sprintf_s(msg, " PC: %02x\n R0: %02x\n R1: %02x\n R2: %02x\n R3: %02x\n R4: %02x\n R5: %02x\n R6: %02x\n R7: %02x\n R8: %02x\n FP(r11): %02x\n IP (r12): %02x\n SP: %02x\n LR: %02x\n CPSR: %02x\n Status: %02x\n, zero: %01x\n, carry: %01x\n, overflow: %01x\n, negative: %01x\n mode: %02x\n",
		*r[15], *r[0], *r[1], *r[2], *r[3], *r[4], *r[5], *r[6], *r[7], *r[8], *r[11], *r[12], *r[13], *r[14], cpsr.val, *r[16], cpsr.zero, cpsr.carry, cpsr.overflow, cpsr.negative, cpsr.mode);

	for (int i = 0; i < 1; i++){
		text.setString(msg);
		text.setPosition(sf::Vector2f(850 + 256, 130 + 12 * i));
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

			if (event.key.code == sf::Keyboard::F)
			{
				debug = !debug;
			}
			if (event.key.code == sf::Keyboard::G)
			{
				if (debug)
					step = true;
			}

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