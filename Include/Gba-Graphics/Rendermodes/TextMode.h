#pragma once
#include <cstdint>

#include "Constants.h"

class TextMode
{

public:
    static void draw(finalImageColored& img,
                     const uint8_t regOffset,
                     const uint32_t line,
                     const bool first);
    void fillImage(finalImagePalettes& imageBase, const uint32_t offset);
    uint32_t* getBG();
};
