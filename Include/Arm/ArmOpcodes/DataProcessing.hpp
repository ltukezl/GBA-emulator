#pragma once

#include <cassert>
#include <cstdint>
#include <format>

#include "Arm/ArmOpcodes/Branch.hpp"
#include "arm/ArmOpcodes/Mrs.hpp"
#include "arm/ArmOpcodes/Msr_imm.hpp"
#include "CommonOperations/GbaStrings.hpp"
#include "cplusplusRewrite/BarrelShifter.h"
#include "cplusplusRewrite/HwRegisters.h"
#include "cplusplusRewrite/MathOps.h"
#include "cplusplusRewrite/Shifts.h"

class DataProcessing
{
public:
    struct DataProcessingOpcode
    {
        uint32_t immediate: 8;
        uint32_t rotate: 4;
        uint32_t rd: 4;
        uint32_t rn: 4;
        uint32_t set_sign: 1;
        uint32_t operation: 4;
        uint32_t reserved: 3;
        uint32_t cond: 4;
    };

    static constexpr std::array conditionFunctions = {
        mathOps::And::calcConditions, mathOps::Eor::calcConditions,
        mathOps::Sub::calcConditions, mathOps::Rsb::calcConditions,
        mathOps::Add::calcConditions, mathOps::Adc::calcConditions,
        mathOps::Sbc::calcConditions, mathOps::Rsc::calcConditions,
        mathOps::Tst::calcConditions, mathOps::Teq::calcConditions,
        mathOps::Cmp::calcConditions, mathOps::Cmn::calcConditions,
        mathOps::Orr::calcConditions, mathOps::Mov::calcConditions,
        mathOps::Bic::calcConditions, mathOps::Mvn::calcConditions};

    static constexpr std::array math_strings = {
        "AND", "EOR", "SUB", "RSB", "ADD", "ADC", "SBC", "RSC",
        "TST", "TEQ", "CMP", "CMN", "ORR", "MOV", "BIC", "MVN"};

    static constexpr DataProcessingOpcode fromOpcode(const uint32_t opcode)
    {
        return {
            .immediate = static_cast<uint32_t>((opcode >> 0x00U) & 0xFFU),
            .rotate = static_cast<uint32_t>((opcode >> 0x04U) & 0xFU),
            .rd = static_cast<uint32_t>((opcode >> 0x0CU) & 0xFU),
            .rn = static_cast<uint32_t>((opcode >> 0x10U) & 0xFU),
            .set_sign = static_cast<uint32_t>((opcode >> 0x14U) & 0x1U),
            .operation = static_cast<uint32_t>((opcode >> 0x15U) & 0xFU),
            .reserved = static_cast<uint32_t>((opcode >> 0x19U) & 0x7U),
            .cond = static_cast<uint32_t>((opcode >> 0x1CU) & 0xFU),
        };
    }

    static constexpr bool is_register_register_rotate(const uint32_t opcode)
    {
        const auto opcodeStruct = fromOpcode(opcode);
        return ((opcodeStruct.reserved == 0) &&
                ((opcodeStruct.immediate & (1U << 4U)) != 0U) &&
                ((opcodeStruct.immediate & (1U << 7U)) == 0U));
    }

    static constexpr bool is_register_immediate_rotate(const uint32_t opcode)
    {
        const auto opcodeStruct = fromOpcode(opcode);
        return ((opcodeStruct.reserved == 0) &&
                ((opcodeStruct.immediate & (1U << 4U)) == 0U));
    }

    static constexpr bool is_immediate_rotate(const uint32_t opcode)
    {
        const auto opcodeStruct = fromOpcode(opcode);
        return (opcodeStruct.reserved == 1U);
    }

    static constexpr bool isThisOpcode(const uint32_t opcode)
    {
        return is_register_register_rotate(opcode) ||
            is_register_immediate_rotate(opcode) || is_immediate_rotate(opcode);
    }

    static constexpr auto mask(const uint32_t opcode)
    { return (opcode & (0x3f << 20)) | (opcode & (0x9 << 4)); }

    template<uint32_t opcode_iter>
    static void execute(Registers& regs, const uint32_t opcode)
    {
        if (MsrImmediate::isThisOpcode(opcode)) {
            MsrImmediate::execute(regs, opcode);
            return;
        } else if (MRS::isThisOpcode(opcode)) {
            MRS::execute(regs, opcode);
            return;
        } else if (branches::ArmBranchAndExhange::isThisOpcode(opcode)) {
            branches::ArmBranchAndExhange::execute(regs, opcode);
            return;
        }
        const auto op = fromOpcode(opcode);
        constexpr auto c_op = fromOpcode(opcode_iter);
        uint32_t result = 0;
        uint32_t operand1 = regs[op.rn];

        if (is_register_register_rotate(opcode_iter) && op.rn == 15) {
            operand1 += 8;
        } else if (op.rn == 15) {
            operand1 += 4;
        }

        uint32_t operand2 = 0;

        uint32_t& dest = regs[op.rd];

        const auto barrel_shifter_func = [c_op]() constexpr {
            if constexpr (c_op.reserved == 1) {
                return ImmediateRotater::calculate;
            } else if constexpr ((c_op.reserved == 0) &&
                                 (c_op.immediate & (1 << 4)) == 0) {
                return RegisterWithImmediateShifter::calculate;
            } else {

                return RegisterWithRegisterShifter::calculate;
            }
        }();

        switch (c_op.operation) {
            case 0:
                operand2 = barrel_shifter_func(regs, opcode, op.set_sign);
                result =
                    mathOps::And::calculate(regs.m_cpsr, operand1, operand2);
                dest = result;
                break;
            case 1:
                operand2 = barrel_shifter_func(regs, opcode, op.set_sign);
                result =
                    mathOps::Eor::calculate(regs.m_cpsr, operand1, operand2);
                dest = result;
                break;
            case 2:
                operand2 = barrel_shifter_func(regs, opcode, op.set_sign);
                result =
                    mathOps::Sub::calculate(regs.m_cpsr, operand1, operand2);
                dest = result;
                break;
            case 3:
                operand2 = barrel_shifter_func(regs, opcode, 0);
                result =
                    mathOps::Rsb::calculate(regs.m_cpsr, operand1, operand2);
                dest = result;
                break;
            case 4:
                operand2 = barrel_shifter_func(regs, opcode, op.set_sign);
                result =
                    mathOps::Add::calculate(regs.m_cpsr, operand1, operand2);
                dest = result;
                break;
            case 5:
                operand2 = barrel_shifter_func(regs, opcode, 0);
                result =
                    mathOps::Adc::calculate(regs.m_cpsr, operand1, operand2);
                dest = result;
                break;
            case 6:
                operand2 = barrel_shifter_func(regs, opcode, 0);
                result =
                    mathOps::Sbc::calculate(regs.m_cpsr, operand1, operand2);
                dest = result;
                break;
            case 7: // RSC
                operand2 = barrel_shifter_func(regs, opcode, 0);
                result =
                    mathOps::Rsc::calculate(regs.m_cpsr, operand1, operand2);
                dest = result;
                break;
            case 8:
                operand2 = barrel_shifter_func(regs, opcode, 1);
                result =
                    mathOps::Tst::calculate(regs.m_cpsr, operand1, operand2);
                mathOps::Tst::calcConditions(regs.m_cpsr, result, operand1,
                                             operand2);
                break; // no writeback
            case 9:
                operand2 = barrel_shifter_func(regs, opcode, 1);
                result =
                    mathOps::Teq::calculate(regs.m_cpsr, operand1, operand2);
                mathOps::Teq::calcConditions(regs.m_cpsr, result, operand1,
                                             operand2);
                break;
            case 10:
                operand2 = barrel_shifter_func(regs, opcode, 1); // don't care
                result =
                    mathOps::Cmp::calculate(regs.m_cpsr, operand1, operand2);
                mathOps::Cmp::calcConditions(regs.m_cpsr, result, operand1,
                                             operand2);
                break;
            case 11:
                operand2 = barrel_shifter_func(regs, opcode, 1); // don't care
                result =
                    mathOps::Cmn::calculate(regs.m_cpsr, operand1, operand2);
                mathOps::Cmn::calcConditions(regs.m_cpsr, result, operand1,
                                             operand2);
                break;
            case 12:
                operand2 = barrel_shifter_func(regs, opcode, op.set_sign);
                result =
                    mathOps::Orr::calculate(regs.m_cpsr, operand1, operand2);
                dest = result;
                break;
            case 13:
                operand2 = barrel_shifter_func(regs, opcode, op.set_sign);
                result =
                    mathOps::Mov::calculate(regs.m_cpsr, operand1, operand2);
                dest = result;
                break;
            case 14:
                operand2 = barrel_shifter_func(regs, opcode, op.set_sign);
                result =
                    mathOps::Bic::calculate(regs.m_cpsr, operand1, operand2);
                dest = result;
                break;
            case 15:
                operand2 = barrel_shifter_func(regs, opcode, op.set_sign);
                result =
                    mathOps::Mvn::calculate(regs.m_cpsr, operand1, operand2);
                dest = result;
                break;
            default: assert(false); // handle invalid instruction
        }

        if constexpr (c_op.set_sign) {
            conditionFunctions[c_op.operation](regs.m_cpsr, result, operand1,
                                               operand2);
        }

        if (op.rd == 15 && c_op.set_sign) {
            regs.m_cpsr.val = regs[16];
            regs.updateMode(regs.m_cpsr.mode);
        }
    }

    static auto disassemble(const uint32_t opcode)
    {
        if (branches::ArmBranchAndExhange::isThisOpcode(opcode)) {
            return branches::ArmBranchAndExhange::disassemble(opcode);
        } else if (MRS::isThisOpcode(opcode)) {
            return MRS::disassemble(opcode);
        }

        const auto op = fromOpcode(opcode);

        const auto operation = math_strings[op.operation];
        const auto condition = condition_strings[op.cond];
        const auto is_signed = op.set_sign ? "S" : "";
        std::string shifter_disasm = "";

        if (op.reserved == 1) {
            shifter_disasm = ImmediateRotater::disassemble(opcode);
        } else if ((op.reserved == 0) && (op.immediate & (1 << 4)) == 0) {
            shifter_disasm = RegisterWithImmediateShifter::disassemble(opcode);
        } else {
            shifter_disasm = RegisterWithRegisterShifter::disassemble(opcode);
        }

        if (op.operation == 0b1111 || op.operation == 0b1101) {
            return std::format("{}{}{} R{}, #{}", operation, condition,
                               is_signed, op.rd, shifter_disasm);
        } else if (op.operation == 0b1000 || op.operation == 0b1001 ||
                   op.operation == 0b1010 || op.operation == 0b1011) {
            return std::format("{}{} R{}, #{}", operation, condition, op.rn,
                               shifter_disasm);
        } else {
            return std::format("{}{}{} R{}, R{}, #{}", operation, condition,
                               is_signed, op.rd, op.rn, shifter_disasm);
        }
    }
};
