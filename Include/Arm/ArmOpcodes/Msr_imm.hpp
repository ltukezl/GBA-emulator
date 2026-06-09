#pragma once

#include <cassert>
#include <cstdint>
#include <format>

#include "CommonOperations/GbaStrings.hpp"
#include "cplusplusRewrite/BarrelShifter.h"
#include "cplusplusRewrite/HwRegisters.h"
#include "cplusplusRewrite/MathOps.h"
#include "cplusplusRewrite/Shifts.h"

#include <print>

class MsrImmediate
{
public:
    struct MsrImmediate_t
    {
        uint32_t source_operand: 12;
        uint32_t reserved1: 4;
        uint32_t ctl: 1;
        uint32_t status: 1;
        uint32_t extensions: 1;
        uint32_t flg: 1;
        uint32_t reserved2: 2;
        uint32_t destination: 1;
        uint32_t reserved3: 5;
        uint32_t cond: 4;
    };

    static constexpr MsrImmediate_t fromOpcode(const uint32_t opcode)
    {
        return {
            .source_operand = static_cast<uint32_t>((opcode >> 0x00U) & 0xFFFU),
            .reserved1 = static_cast<uint32_t>((opcode >> 0x0CU) & 0x0FU),
            .ctl = static_cast<uint32_t>((opcode >> 0x10U) & 0x01U),
            .status = static_cast<uint32_t>((opcode >> 0x11U) & 0x01U),
            .extensions = static_cast<uint32_t>((opcode >> 0x12U) & 0x01U),
            .flg = static_cast<uint32_t>((opcode >> 0x13U) & 0x01U),
            .reserved2 = static_cast<uint32_t>((opcode >> 0x14U) & 0x03U),
            .destination = static_cast<uint32_t>((opcode >> 0x16U) & 0x01U),
            .reserved3 = static_cast<uint32_t>((opcode >> 0x17U) & 0x1FU),
            .cond = static_cast<uint32_t>((opcode >> 0x1CU) & 0x0FU),
        };
    }

    static constexpr bool isThisOpcode(const uint32_t opcode)
    {
        const auto opcodeStruct = fromOpcode(opcode);
        return (opcodeStruct.reserved1 == 0b1111) &&
            (opcodeStruct.reserved2 == 0b10) &&
            (opcodeStruct.reserved3 == 0b00110);
    }

    static void execute(Registers& regs, const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        const auto operand = ImmediateRotater::calculate(regs, opcode, 0);
        const CPSR_t tmp_cpsr{operand};

        if (op.destination) {
            CPSR_t tmp_cpsr1{};
            tmp_cpsr1.val = regs[16];

            if (regs.m_cpsr.mode != CpuModes_t::EUSR &&
                regs.m_cpsr.mode != CpuModes_t::ESYS) {
                if (op.ctl) {
                    tmp_cpsr1.FIQDisable = tmp_cpsr.FIQDisable;
                    tmp_cpsr1.IRQDisable = tmp_cpsr.IRQDisable;
                    tmp_cpsr1.mode = tmp_cpsr.mode;
                }
                if (op.flg) {
                    tmp_cpsr1.zero = tmp_cpsr.zero;
                    tmp_cpsr1.overflow = tmp_cpsr.overflow;
                    tmp_cpsr1.carry = tmp_cpsr.carry;
                    tmp_cpsr1.negative = tmp_cpsr.negative;
                }
                regs[16] = tmp_cpsr1.val;
            }
        } else {
            if (regs.m_cpsr.mode != CpuModes_t::EUSR && op.ctl) {
                regs.m_cpsr.FIQDisable = tmp_cpsr.FIQDisable;
                regs.m_cpsr.IRQDisable = tmp_cpsr.IRQDisable;
                regs.m_cpsr.mode = tmp_cpsr.mode;
                regs.updateMode(tmp_cpsr.mode);
            }
            if (op.flg) {
                regs.m_cpsr.zero = tmp_cpsr.zero;
                regs.m_cpsr.overflow = tmp_cpsr.overflow;
                regs.m_cpsr.carry = tmp_cpsr.carry;
                regs.m_cpsr.negative = tmp_cpsr.negative;
            }
        }
    }
    static auto disassemble(const uint32_t opcode) { return ""; }
};
