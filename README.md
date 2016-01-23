# GBA-emulator
hobby project trying to make working gba emulator
CPU is arm7-tdmi
Thumb instruction set seems to be working.
Currently it's running factorial of 5 program on thumb.
ARM instructions are still little incomplete. MUL and SWP are not implemented.
Emulator is still missing software interrupts, ARM opcodes for switching and handling status register
and different register banks for different modes (supervisor, fig, irq, etc).

compiled on VS2013
