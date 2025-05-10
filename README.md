# GBA-emulator
hobby project trying to make working gba emulator.

CPU is arm7-tdmi

ARM instructions are still little incomplete. MUL LONG and SWP are not implemented, neither is any of the co processor operations since GBA does not have them.

Emulator is still missing software interrupts and periphials.

This is still baremetal implementation. No IO is being emulated. Started to work with displays, but haven't found good documentation for them.
I'm using SFML to implement displays

compiled on VS2013
