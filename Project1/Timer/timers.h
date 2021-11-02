#ifndef TIMERS_H
#define TIMERS_H
#include <stdint.h>

bool timerReloadWrite(uint32_t addr, uint32_t val);
bool reloadCounter(uint32_t addr, uint32_t val);
void updateTimers(uint32_t cycles);

#endif