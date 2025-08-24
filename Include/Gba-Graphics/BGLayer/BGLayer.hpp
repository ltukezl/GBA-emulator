#ifndef BGLAYER_H
#define BGLAYER_H

#include <array>
#include <cstdint>

#include "Constants.h"
#include "Memory/memoryMappedIO.h"

class BGLayer
{
public:
    constexpr BGLayer(const uint32_t bgNumber, BgCnt* const controlRegister) :
        m_bg{bgNumber}, m_reg{*controlRegister}
    {
        pixels = std::make_unique<finalImageColored>();
        size_t cntr = 0;
        for (auto& arr: *pixels) {
            for (auto& px: arr) {
                if (bgNumber == 2) {
                    px.r = 255;
                }
                if (bgNumber == 3) {
                    px.g = 255;
                    if (cntr % 2) {
                        px.a = 0;
                    } else {
                        px.a = 255;
                    }
                    cntr++;
                }
            }
        }
    }
    std::unique_ptr<finalImageColored> pixels;
private:
    const uint32_t m_bg;
    BgCnt& m_reg;
};

#endif
