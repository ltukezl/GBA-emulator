#pragma once

#include <cstdint>

class GameDisplay;

class VideoCycleCounter
{
public:
    void increment(GameDisplay& disp);
    bool m_draw_frame = false;
    uint16_t m_counter = 0;
private:
};
