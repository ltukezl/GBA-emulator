#ifndef BGLAYER_H
#define BGLAYER_H

#include <cstdint>

#include "Gba-Graphics/Rendermodes/TextMode.h"
#include "Gba-Graphics/Sprites/Sprite.h"
#include "Memory/memoryMappedIO.h"

class BGLayer
{
public:
    constexpr BGLayer(const uint32_t bgNumber, BgCnt* const controlRegister) :
        m_bg{bgNumber}, m_reg{controlRegister}
    {
    }

    auto operator<=>(const BGLayer& other) const
    {
        if (auto cmp = m_reg->priority <=> other.m_reg->priority; cmp != 0) {
            return cmp;
        }
        return m_bg <=> other.m_bg;
    }

    auto operator<=>(const Sprite& other) const
    {
        if (auto cmp = m_reg->priority <=> other.get_priority(); cmp != 0) {
            return cmp;
        }
        return std::strong_ordering::greater;
    }

    void draw_text_mode(finalImageColored& img,
                        const uint32_t line,
                        const bool first) const
    { TextMode::draw(img, 2 * m_bg, line, false); }

    bool is_enabled() const
    {
        if (m_bg == 0) {
            return displayCtrl->bg0Display;
        }
        if (m_bg == 1) {
            return displayCtrl->bg1Display;
        }
        if (m_bg == 2) {
            return displayCtrl->bg2Display;
        }
        if (m_bg == 3) {
            return displayCtrl->bg3Display;
        }
        return false;
    }

private:
    uint32_t m_bg;
    BgCnt* m_reg;
};

#endif
