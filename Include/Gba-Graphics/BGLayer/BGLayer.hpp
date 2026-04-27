#ifndef BGLAYER_H
#define BGLAYER_H

#include <cstdint>

#include "Constants.h"
#include "Memory/memoryMappedIO.h"

class BGLayer
{
public:
    constexpr BGLayer(const uint32_t bgNumber, BgCnt* const controlRegister) :
        m_bg{bgNumber}, m_reg{controlRegister}
    { pixels = std::make_unique<finalImageColored>(); }
    std::unique_ptr<finalImageColored> pixels;

    bool operator<(const BGLayer& other) const noexcept
    {
        if (m_reg->priority == other.m_reg->priority) {
            return m_bg < other.m_bg;
        }
        return m_reg->priority < other.m_reg->priority;
    }

private:
    uint32_t m_bg;
    BgCnt* m_reg;
};

#endif
