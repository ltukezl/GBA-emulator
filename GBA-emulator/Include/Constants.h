#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <cstdint>
#include "Gba-Graphics/Tile/Tile.h"
#include "Gba-Graphics/Palette/RgbaPalette.h"

extern uint32_t systemROMStart;
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
extern bool debug;

using finalImagePalettes = std::array<std::array<Tile::BitmapBit, 240>, 160>;
using finalImageColored = std::array<std::array<RgbaPalette::GBAColor, 240>, 160>;

#endif
