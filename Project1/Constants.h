#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <stdint.h>

extern uint32_t systemROMStart;
extern uint32_t ExternalWorkRAMStart;
extern uint32_t InternalWorkRAMStart;
extern uint32_t IoRAMStart;
extern uint32_t PaletteRAMStart;
extern uint32_t VRAMStart;
extern uint32_t OAMStart;
extern uint32_t SP_svc;
extern uint32_t SP_irq;
extern uint32_t SP_usr;

#define SP 13
#define LR 14
#define PC 15

#endif
