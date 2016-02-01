# GBA-emulator
hobby project trying to make working gba emulator.

CPU is arm7-tdmi

Thumb instruction set seems to be working.

ARM instructions are still little incomplete. MUL and SWP are not implemented.

Emulator is still missing software interrupts, ARM opcodes for switching between thumb mode
and different register banks for different modes (supervisor, fig, irq, etc).

This is still baremetal implementation. No IO is being emulated.

compiled on VS2013
