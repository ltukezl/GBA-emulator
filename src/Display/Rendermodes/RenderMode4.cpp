#include "Gba-Graphics/Rendermodes/RenderMode4.h"
#include "Gba-Graphics/Palette/RgbaPalette.h"
#include "Memory/memoryOps.h"

extern RgbaPalette PaletteColours;

void RenderMode4::draw(finalImageColored& img, const uint32_t line)
{
    if (line >= 160) {
        return;
    }
    uint32_t startAddr = 0xa000 * displayCtrl->displayFrame;
    startAddr += 240 * line;

    for (size_t i = 0; i < 240; i++) {
        uint8_t colorIdx = vram.getMemoryPtr()[startAddr++];
        auto paletteColor = PaletteColours.colorFromIndex(colorIdx);
        img[line][i] = paletteColor;
    }
}
