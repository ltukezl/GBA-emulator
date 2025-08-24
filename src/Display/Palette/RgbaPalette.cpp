#include "Gba-Graphics/Palette/RgbaPalette.h"
#include "Memory/memoryMappedIO.h"
#include "Memory/memoryOps.h"
#include <cstdint>

void RgbaPalette::updatePalette()
{
    if (!paletteram.m_accessed) {
        return;
    }

    for (size_t i = 0; i < paletteram.m_accessedPaletteColour.size(); i++) {
        if (!paletteram.m_accessedPaletteColour[i]) {
            continue;
        }
        const ColorPaletteRam* colorPaletteRam = &m_colorStartAddress[i];
        const uint32_t redScaled = colorPaletteRam->red * _scalar;
        const uint32_t greenScaled = colorPaletteRam->green * _scalar;
        const uint32_t blueScaled = colorPaletteRam->blue * _scalar;
        paletteColorArray.paletteColorArray_linear[i].r = redScaled;
        paletteColorArray.paletteColorArray_linear[i].g = greenScaled;
        paletteColorArray.paletteColorArray_linear[i].b = blueScaled;
        paletteColorArray.paletteColorArray_linear[i].a = 255;
    }
    paletteram.m_accessedPaletteColour.reset();
    paletteram.clearAccess();
}

RgbaPalette::GBAColor RgbaPalette::colorFromIndex(const uint32_t index) const
{
    return paletteColorArray.paletteColorArray_linear[index];
}
RgbaPalette::GBAColor RgbaPalette::colorFromIndex(const uint32_t y,
                                                  const uint32_t x) const
{
    return paletteColorArray.paletteColorArray_2D[y][x];
}

const uint8_t* RgbaPalette::getPalette()
{
    return reinterpret_cast<const uint8_t*>(
        paletteColorArray.paletteColorArray_linear.data());
}
