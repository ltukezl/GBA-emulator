#include "Gba-Graphics/Rendermodes/RenderMode5.h"
#include "Gba-Graphics/Palette/RgbaPalette.h"
#include "Memory/memoryOps.h"

extern RgbaPalette PaletteColours;

void RenderMode5::draw(finalImageColored& img, const uint32_t line)
{
    if (line >= 160) {
        return;
    }
    if (line >= 128) {
        for (size_t i = 0; i < 240; i++) {
            img[line][i] = PaletteColours.colorFromIndex(0);
        }
        return;
    }
    uint32_t startAddr = 0xa000 * displayCtrl->displayFrame;
    startAddr += 160 * line * 2;

    for (size_t i = 0; i < 160; i++) {
        ColorPaletteRam* colorPaletteRam = (ColorPaletteRam*)&vram[startAddr];
        RgbaPalette::GBAColor color;
        color.r = colorPaletteRam->red * _scalar;
        color.g = colorPaletteRam->green * _scalar;
        color.b = colorPaletteRam->blue * _scalar;
        color.a = 255;
        startAddr += 2;
        img[line][i] = color;
    }

    for (size_t i = 160; i < 240; i++) {
        img[line][i] = PaletteColours.colorFromIndex(0);
    }
}
