#include "Gba-Graphics/Tile/Tile.h"
#include "Memory/MemoryOps.h"
#include "Memory/memoryMappedIO.h"
#include <iostream>

extern RgbaPalette PaletteColours;

Tile::Tile(uint32_t addr, bool isObj) {
	uint32_t startAddr = 0;
	uint8_t offset = 0;
	
	for (int y = 0; y < 8; y++) {
		uint32_t row = rawLoad32(vram.getMemoryPtr(), addr + startAddr);
		for (size_t pixel = 0; pixel < 8; pixel++) {
			uint16_t color = (row & 0xf);
			_tile.grid[y][pixel].index = color;
			row >>= 4;
		}
		startAddr += 4;
	}
	
	if (!isObj && addr < 0xC000) {
		for (size_t y = 0; y < 8; y++) {
			for (size_t pixel = 0; pixel < 8; pixel++) {
				uint8_t color = rawLoad8(&vram[0], addr * 2 + offset  );
				auto palette = color / 16;
				auto index = color % 16;
				_tile8bit.grid[y][pixel].palette = palette;
				_tile8bit.grid[y][pixel].index = index;
				offset++;
			}
		}
	}
}

Tile::GBATile& Tile::getTile(bool is8Bit, uint8_t palette) {
	if (is8Bit)
		return _tile8bit;
	//unrolled for performance
	for (int i = 0; i < 8; i++) {
		_tile.grid[i][0].palette = palette;
		_tile.grid[i][1].palette = palette;
		_tile.grid[i][2].palette = palette;
		_tile.grid[i][3].palette = palette;
		_tile.grid[i][4].palette = palette;
		_tile.grid[i][5].palette = palette;
		_tile.grid[i][6].palette = palette;
		_tile.grid[i][7].palette = palette;
	}
	return _tile;
}