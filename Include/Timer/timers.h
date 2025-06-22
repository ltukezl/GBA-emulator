#pragma once
#include <cstdint>

bool timerReloadWrite(uint32_t addr, uint32_t val);
bool reloadCounter(uint32_t addr, uint32_t val);
void updateTimers(uint64_t cycles);
