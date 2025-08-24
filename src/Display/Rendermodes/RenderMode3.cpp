#include "Gba-Graphics/Rendermodes/RenderMode3.h"
#include "Gba-Graphics/Palette/RgbaPalette.h"
#include "Memory/memoryOps.h"

void RenderMode3::draw(finalImageColored& img, const uint32_t line)
{
    if (line >= 160) {
        return;
    }
    uint32_t startAddr = line * 240 * 2;

    for (size_t i = 0; i < 240; i++) {
        ColorPaletteRam* colorPaletteRam = (ColorPaletteRam*)&vram[startAddr];
        uint32_t redScaled = colorPaletteRam->red * _scalar;
        uint32_t greenScaled = colorPaletteRam->green * _scalar;
        uint32_t blueScaled = colorPaletteRam->blue * _scalar;
        img[line][i].r = redScaled;
        img[line][i].b = blueScaled;
        img[line][i].g = greenScaled;
        img[line][i].a = 255;
        startAddr += 2;
    }
}
