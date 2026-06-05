#pragma once

#include <cstdint>
#include <format>

#include "CommonOperations/GbaStrings.hpp"
#include "cplusplusRewrite/HwRegisters.h"

class Swap
{
public:
    struct SwapOpcode
    {
        uint32_t rm: 4;
        uint32_t reserved1: 4;
        uint32_t reserved2: 4;
        uint32_t rd: 4;
        uint32_t rn: 4;
        uint32_t reserved3: 2;
        uint32_t byte: 1;
        uint32_t reserved4: 5;
        uint32_t cond: 4;
    };

    static constexpr SwapOpcode fromOpcode(const uint32_t opcode)
    {
        return {.rm = static_cast<uint32_t>((opcode >> 0x00) & 0xF),
                .reserved1 = static_cast<uint32_t>((opcode >> 0x04) & 0xF),
                .reserved2 = static_cast<uint32_t>((opcode >> 0x08) & 0xF),
                .rd = static_cast<uint32_t>((opcode >> 0x0C) & 0xF),
                .rn = static_cast<uint32_t>((opcode >> 0x10) & 0xF),
                .reserved3 = static_cast<uint32_t>((opcode >> 0x14) & 0x3),
                .byte = static_cast<uint32_t>((opcode >> 0x16) & 0x1),
                .reserved4 = static_cast<uint32_t>((opcode >> 0x17) & 0x1F),
                .cond = static_cast<uint32_t>((opcode >> 0x1C) & 0xF)};
    }

    static constexpr bool isThisOpcode(const uint32_t opcode)
    {
        const auto opcodeStruct = fromOpcode(opcode);
        return (opcodeStruct.reserved1 == 9) && (opcodeStruct.reserved3 == 0) &&
            (opcodeStruct.reserved4 == 2);
    }

    static constexpr auto mask(const uint32_t opcode)
    { return opcode & (1 << 22); }

    template<uint32_t opcode_iter>
    static void execute(Registers& regs, const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        constexpr auto c_op = fromOpcode(opcode_iter);
        uint32_t swap_val_address = regs[op.rn];
        if (op.rn == 15) {
            swap_val_address += 8;
        }
        uint32_t swap_val = regs[op.rm];
        if (op.rm == 15) {
            swap_val += 8;
        }

        if constexpr (c_op.byte) {
            const uint32_t tmp = loadFromAddress(swap_val_address);
            writeToAddress(swap_val_address, swap_val);
            regs[op.rd] = tmp;
        } else {
            const uint32_t tmp = loadFromAddress32(swap_val_address);
            writeToAddress32(swap_val_address, swap_val);
            regs[op.rd] = tmp;
        }
    }

    static auto disassemble(const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        const auto byte = (op.byte == 1) ? "B" : "";
        const auto condition = condition_strings[op.cond];

        return std::format("SWP{}{} {},{},[{}]", byte, condition, op.rd, op.rm,
                           op.rn);
    }
};
