#pragma once
#include "Constants.h"
#include <cstdint>

class RenderMode5
{
private:
    static constexpr uint32_t _scalar = 255 / 31;

public:
    static void draw(finalImageColored& img, const uint32_t line);
};
