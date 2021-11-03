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

	paletteTexture.create(16, 32);
	paletteTile.create(16, 32, sf::Color::Black);

	paletteSprite.setTexture(paletteTexture, true);
	paletteSprite.setPosition(0, 0);
	paletteSprite.setScale(16.0, 16.0);

	tmpTile.create(8, 8);
	tmpTile.createMaskFromColor(sf::Color(0, 0, 0));
	tileMap1Texture.create(256, 513);

	tilemapSprite.setTexture(tileMap1Texture, true);
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

	gameTXT.create(512, 512);
	gameTXT.setRepeated(true);
}

void Display::scanPalettes(){
	int startAddr = 0;
	int colorPtr = 0;
	const int scalar = 255 / 31;

	//for bg palettes
	for (int i = 0; i < 32; i++)
		for (int k = 0; k < 16; k++){
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

sf::Image& Display::calculate4BitTile(uint32_t base, BgTile* tile){
	uint32_t pixeloffset = 0;
	for (int y = 0; y < 8; y++){
		int row = loadFromAddress32(0x6000000 + base + tile->tileNumber * 0x20 + pixeloffset, true);
		for (int pixel = 0; pixel < 8; pixel++){
			int color = (row & 0xf);
			bgTile[4 * 8 * y + 4 * pixel + 0] = PaletteColors[16 * tile->paletteNum + color].r;
			bgTile[4 * 8 * y + 4 * pixel + 1] = PaletteColors[16 * tile->paletteNum + color].g;
			bgTile[4 * 8 * y + 4 * pixel + 2] = PaletteColors[16 * tile->paletteNum + color].b;
			bgTile[4 * 8 * y + 4 * pixel + 3] = PaletteColors[16 * tile->paletteNum + color].a;
			row >>= 4;
		}
		pixeloffset += 4;
	}
	tmpTile.create(8, 8, bgTile);
	if (tile->horizontalFlip)
		tmpTile.flipHorizontally();
	if (tile->VerticalFlip)
		tmpTile.flipVertically();

	return tmpTile;
}

sf::Image& Display::calculate8BitTile(uint32_t base, BgTile* tile){
	uint32_t offset = 0x6000000 + base + tile->tileNumber * 0x40;
	for (int y = 0; y < 8; y++){
		for (int pixel = 0; pixel < 8; pixel++){
			int color = loadFromAddress(offset, true);
			bgTile[4 * 8 * y + 4 * pixel + 0] = PaletteColors[color].r;
			bgTile[4 * 8 * y + 4 * pixel + 1] = PaletteColors[color].g;
			bgTile[4 * 8 * y + 4 * pixel + 2] = PaletteColors[color].b;
			bgTile[4 * 8 * y + 4 * pixel + 3] = PaletteColors[color].a;
			offset++;
		}
	}
	tmpTile.create(8, 8, bgTile);
	if (tile->horizontalFlip)
		tmpTile.flipHorizontally();
	if (tile->VerticalFlip)
		tmpTile.flipVertically();

	return tmpTile;
}

void Display::fillTiles(uint32_t regOffset){
	if (!VRAMupdated){
		display->draw(tilemapSprite);
		return;
	}
	BgCnt* bgCnt = (BgCnt*)&IoRAM[8 + regOffset];
	int startAddr = 0;

	/*creates tilemaps 1 and 2*/
	for (int tileY = 0; tileY < 64; tileY++)
		for (int tileX = 0; tileX < 32; tileX++){
			for (int y = 0; y < 8; y++){
				int row = rawLoad32(VRAM, startAddr);
				for (int pixel = 0; pixel < 8; pixel++){
					int color = (row & 0xf);
					tmpTile.setPixel(pixel, y, PaletteColors[color]);
					row >>= 4;
				}
				startAddr += 4;
			}
			tileMap[32 * tileY + tileX] = tmpTile;
		}

	/*draws tilemaps 1 and 2 to screen*/

	for (int i = 0; i < 64; i++)
		for (int k = 0; k < 32; k++){
			sf::Image tile = tileMap[32 * i + k];
			tileMap1Texture.update(tile, 8 * k, 8 * i);
		}

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
					calculate4BitTile(tileBaseBlock, tile);
				else
					calculate8BitTile(tileBaseBlock, tile);

				startAddr += 2;
				bgText[regOffset / 2].update(tmpTile, 8 * k, 8 * i);
				
			}
		}

		if (bgCnt->hWide){
			for (int i = 0; i < 32; i++){
				for (int k = 0; k < 32; k++){
					BgTile* tile = (BgTile*)&VRAM[startAddr];

					if (bgCnt->palettes == 0)
						calculate4BitTile(tileBaseBlock, tile);
					else
						calculate8BitTile(tileBaseBlock, tile);

					bgText[regOffset / 2].update(tmpTile, 256 + 8 * k, 8 * i);
					startAddr += 2;
				}
			}
		}
		if (bgCnt->vWide){
			for (int i = 0; i < 32; i++){
				for (int k = 0; k < 32; k++){
					BgTile* tile = (BgTile*)&VRAM[startAddr];

					if (bgCnt->palettes == 0)
						calculate4BitTile(tileBaseBlock, tile);
					else
						calculate8BitTile(tileBaseBlock, tile);

					bgText[regOffset / 2].update(tmpTile, 8 * k, 256 + 8 * i);
					startAddr += 2;
				}
			}
		}
		if (bgCnt->vWide && bgCnt->vWide){
			for (int i = 0; i < 32; i++){
				for (int k = 0; k < 32; k++){
					BgTile* tile = (BgTile*)&VRAM[startAddr];

					if (bgCnt->palettes == 0)
						calculate4BitTile(tileBaseBlock, tile);
					else
						calculate8BitTile(tileBaseBlock, tile);

					bgText[regOffset / 2].update(tmpTile, 256 + 8 * k, 256 + 8 * i);
					startAddr += 2;
				}
			}
		}

		sf::Sprite BG1Sprite(bgText[regOffset / 2], sf::IntRect(0, 0, size_x, size_y));
		BG1Sprite.setPosition(514, 256 * (regOffset / 2));
		BG1Sprite.setScale(256 / size_x, 256 / size_y);

		display->draw(BG1Sprite);
	}

	else if (displayCtrl->bgMode == 3){
		uint32_t startAddr = bgCnt->bgBaseblock * 0x800;
		sf::Image tile;
		sf::Texture mode3Texture;
		mode3Texture.create(240, 160);
		for (int k = 0; k < 160; k++)
			for (int i = 0; i < 240; i++){
				ColorPaletteRam* colorPaletteRam = (ColorPaletteRam*)&VRAM[startAddr];
				int redScaled = colorPaletteRam->red * scalar;
				int greenScaled = colorPaletteRam->green * scalar;
				int blueScaled = colorPaletteRam->blue * scalar;
				sf::Color color(redScaled, greenScaled, blueScaled);
				tile.create(1, 1, color);
				mode3Texture.update(tile, i, k);

				startAddr += 2;
			}
		sf::Sprite BG1Sprite;
		BG1Sprite.setTexture(mode3Texture, true);
		BG1Sprite.setPosition(514, 256 * (regOffset / 2));
		BG1Sprite.setScale(256 / size_x, 256 / size_y);
		display->draw(BG1Sprite);
	}
	else if (displayCtrl->bgMode == 4){
		uint32_t startAddr = bgCnt->bgBaseblock * 0x800;
		sf::Image tile;
		for (int k = 0; k < 160; k++)
			for (int i = 0; i < 240; i++){
				uint8_t colorIdx = loadFromAddress(0x6000000 + startAddr);
				tile.create(1, 1, PaletteColors[colorIdx]);
				bgText[regOffset / 2].update(tile, i, k);

				startAddr += 1;
			}
		sf::Sprite BG1Sprite(bgText[regOffset / 2], sf::IntRect(0, 0, size_x, size_y));
		BG1Sprite.setPosition(514, 256 * (regOffset / 2));
		BG1Sprite.setScale(256 / size_x, 256 / size_y);

		display->draw(BG1Sprite);
	}

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
			for (int y = 0; y < 8; y++){
				int row = loadFromAddress32(startAddr, true);
				for (int pixel = 0; pixel < 8; pixel++){
					int color = (row & 0xf);
					tmpTile.setPixel(pixel, y, PaletteColors[256 + color]);
					row >>= 4;
				}
				startAddr += 4;
			}
			objMap[32 * tileY + tileX] = tmpTile;
		}


	/*draws sprites 1 and 2 to screen*/

	for (int i = 0; i < 32; i++)
		for (int k = 0; k < 32; k++){
			sf::Image tile = objMap[32 * i + k];
			objMapTexture.update(tile, 8 * k, 8 * i);
		}

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
	//sf::Sprite img = sf::Sprite(gameTXT.getTexture());
	img.setPosition(0, 512);
	img.scale(2, 2);
	display->draw(img);
}

void Display::updatePalettes(){
	display->clear(sf::Color::Black);

	scanPalettes();
	fillObjects(0);

	if (displayCtrl->bg0Display){
		fillTiles(0);
		fillBG(0);
	}
	if (displayCtrl->bg1Display){
		fillTiles(2);
		fillBG(2);
	}
	if (displayCtrl->bg2Display){
		fillTiles(4);
		fillBG(4);
	}
	if (displayCtrl->bg3Display){
		fillTiles(6);
		fillBG(6);
	}

	appendBGs();

#ifdef ENABLED

#endif

	sf::Font font;
	font.loadFromFile("arial.ttf");

	sf::Text text;
	text.setFont(font);
	text.setColor(sf::Color::Red);
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