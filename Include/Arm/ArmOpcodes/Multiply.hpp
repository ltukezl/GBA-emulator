#pragma once

#include <array>
#include <bit>
#include <cstdint>
#include <format>

#include "CommonOperations/GbaStrings.hpp"
#include "cplusplusRewrite/HwRegisters.h"

class MultiplyAccumulate
{
public:
    struct MultiplyAccumulateOpcode
    {
        uint32_t operand1: 4;
        uint32_t reserved1: 4;
        uint32_t operand2: 4;
        uint32_t destinationLow: 4;
        uint32_t destinationHigh: 4;
        uint32_t setCondition: 1;
        uint32_t accumulate: 1;
        uint32_t sign: 1;
        uint32_t reserved2: 5;
        uint32_t cond: 4;
    };

    static constexpr MultiplyAccumulateOpcode fromOpcode(const uint32_t opcode)
    {
        return {.operand1 = static_cast<uint32_t>((opcode >> 0x00) & 0xF),
                .reserved1 = static_cast<uint32_t>((opcode >> 0x04) & 0xF),
                .operand2 = static_cast<uint32_t>((opcode >> 0x08) & 0xF),
                .destinationLow = static_cast<uint32_t>((opcode >> 0x0C) & 0xF),
                .destinationHigh =
                    static_cast<uint32_t>((opcode >> 0x10) & 0xF),
                .setCondition = static_cast<uint32_t>((opcode >> 0x14) & 0x1),
                .accumulate = static_cast<uint32_t>((opcode >> 0x15) & 0x1),
                .sign = static_cast<uint32_t>((opcode >> 0x16) & 0x1),
                .reserved2 = static_cast<uint32_t>((opcode >> 0x17) & 0x1F),
                .cond = static_cast<uint32_t>((opcode >> 0x1C) & 0xF)};
    }

    static constexpr bool isThisOpcode(const uint32_t opcode)
    {
        const auto opcodeStruct = fromOpcode(opcode);
        return (opcodeStruct.reserved1 == 9) && (opcodeStruct.reserved2 == 1);
    }

    static constexpr auto mask(const uint32_t opcode)
    { return opcode & (7 << 20); }

    template<uint32_t iterOpcode>
    static void execute(Registers& regs, const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        constexpr auto c_op = fromOpcode(iterOpcode);

        uint64_t operand1 = regs[op.operand1];
        uint64_t operand2 = regs[op.operand2];
        uint64_t operand3 = regs[op.destinationHigh];
        uint64_t operand4 = regs[op.destinationLow];

        if (op.operand1 == 15) {
            operand1 += 8;
        }
        if (op.operand2 == 15) {
            operand2 += 8;
        }
        if (op.destinationHigh == 15) {
            operand3 += 8;
        }
        if (op.destinationLow == 15) {
            operand4 += 8;
        }

        uint64_t result = 0;

        if constexpr (c_op.sign) {
            const auto op1 =
                static_cast<int64_t>(static_cast<int32_t>(operand1));
            const auto op2 =
                static_cast<int64_t>(static_cast<int32_t>(operand2));
            result = std::bit_cast<uint64_t>(op1 * op2);
        } else {
            const auto op1 = static_cast<uint64_t>(operand1);
            const auto op2 = static_cast<uint64_t>(operand2);
            result = op1 * op2;
        }

        if constexpr (c_op.accumulate) {
            uint64_t current = (operand3 << 32) | operand4;
            result += current;
        }

        const uint32_t hiPart = (result >> 32) & 0xFFFF'FFFF;
        const uint32_t loPart = (result >> 0) & 0xFFFF'FFFF;
        regs[op.destinationLow] = loPart;
        regs[op.destinationHigh] = hiPart;

        if constexpr (c_op.setCondition) {
            regs.m_cpsr.zero = result ? 0 : 1;
            regs.m_cpsr.negative = ((result >> 63) & 1);
        }
    }

    static auto disassemble(const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        static constexpr std::array<std::array<const char*, 2>, 2> txt = {
            {{"UMULL", "UMLAL"}, {"SMULL", "SMLAL"}}};
        const auto s = op.setCondition ? "S" : "";

        return std::format("{}{}{} R{},R{}, R{},R{}",
                           txt[op.sign][op.accumulate],
                           condition_strings[op.cond], s, op.destinationLow,
                           op.destinationHigh, op.operand1, op.operand2);
    }
};

class MultiplyLong
{
public:
    struct MultiplyLongOp
    {
        uint32_t operand1: 4;
        uint32_t reserved1: 4;
        uint32_t operand2: 4;
        uint32_t operand3: 4;
        uint32_t destination: 4;
        uint32_t setCondition: 1;
        uint32_t accumulate: 1;
        uint32_t reserved2: 6;
        uint32_t cond: 4;
    };

    static constexpr MultiplyLongOp fromOpcode(const uint32_t opcode)
    {
        return {.operand1 = static_cast<uint32_t>((opcode >> 0x00) & 0xF),
                .reserved1 = static_cast<uint32_t>((opcode >> 0x04) & 0xF),
                .operand2 = static_cast<uint32_t>((opcode >> 0x08) & 0xF),
                .operand3 = static_cast<uint32_t>((opcode >> 0x0C) & 0xF),
                .destination = static_cast<uint32_t>((opcode >> 0x10) & 0xF),
                .setCondition = static_cast<uint32_t>((opcode >> 0x14) & 0x1),
                .accumulate = static_cast<uint32_t>((opcode >> 0x15) & 0x1),
                .reserved2 = static_cast<uint32_t>((opcode >> 0x16) & 0x3F),
                .cond = static_cast<uint32_t>((opcode >> 0x1C) & 0xF)};
    }

    static constexpr bool isThisOpcode(const uint32_t opcode)
    {
        auto opcodeStruct = fromOpcode(opcode);
        return (opcodeStruct.reserved1 == 9) && (opcodeStruct.reserved2 == 0);
    }

    static constexpr auto mask(const uint32_t opcode)
    { return opcode & (3 << 20); }

    template<uint32_t iterOpcode>
    static void execute(Registers& regs, const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        constexpr auto c_op = fromOpcode(iterOpcode);

        uint64_t operand1 = regs[op.operand1];
        uint64_t operand2 = regs[op.operand2];
        uint64_t accumulate_base = regs[op.operand3];

        if (op.operand1 == 15) {
            operand1 += 8;
        }
        if (op.operand2 == 15) {
            operand2 += 8;
        }
        if (op.operand3 == 15) {
            accumulate_base += 8;
        }

        uint64_t result = operand1 * operand2;
        result += c_op.accumulate ? accumulate_base : 0;
        regs[op.destination] = result;
        if constexpr (c_op.setCondition) {
            regs.m_cpsr.zero = result ? 0 : 1;
            regs.m_cpsr.negative = ((result >> 31) & 1);
        }
    }

    static auto disassemble(const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        const auto s = op.setCondition ? "S" : "";

        if (op.accumulate) {
            return std::format("MLA{}{} R{},R{},R{},R{}",
                               condition_strings[op.cond], s, op.destination,
                               op.operand1, op.operand2, op.operand3);
        } else {
            return std::format("MUL{}{} R{},R{},R{}",
                               condition_strings[op.cond], s, op.destination,
                               op.operand1, op.operand2);
        }
    }
};
