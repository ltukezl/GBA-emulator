#pragma once

#include <cstdint>

class GameDisplay;

class VideoCycleCounter
{
public:
    void increment(GameDisplay& disp);
private:
    uint16_t m_counter = 0;
};
