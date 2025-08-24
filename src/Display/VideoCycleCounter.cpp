#include <cstdint>

#include "Display/GameDisplay.hpp"
#include "Display/VideoCycleCounter.hpp"
#include "Memory/memoryMappedIO.h"
#include "Memory/memoryOps.h"

static inline void updateLYC(uint8_t& LYC)
{
    LYC++;
    LCDStatus->vCounter = (LCDStatus->LYC == LYC);
    InterruptFlagRegister->vCounter = LCDStatus->VcounterIRQEn &&
        InterruptEnableRegister->vCounter && LCDStatus->vCounter;
    if (LYC == 228) {
        LYC = 0;
    }
}

void VideoCycleCounter::increment(GameDisplay& disp)
{
    auto& LYC = memoryLayout[4][6];
    m_counter++;
    if (m_counter == 1232) {
        m_counter = 0;
        updateLYC(LYC);
        disp.drawLine(LYC);
    }
    LCDStatus->hblankFlag = m_counter >= 960;
}
