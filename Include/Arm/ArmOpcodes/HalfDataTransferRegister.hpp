#ifndef HDTR_H
#define HDTR_H
#include <cstdint>

#include "cplusplusRewrite/BarrelShifterDecoder.h"
#include "cplusplusRewrite/HwRegisters.h"
#include "HalfDataTransfer.hpp"
#include "Include/Arm/ArmOpcodes/SingleDataTransfer.hpp"

namespace HalfDataTransfer {

// ----------
// PRE FUNCS STORE
// ----------

class HalfDataTransferRPrS
{
public:

    static constexpr bool isThisOpcode(const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        return (op.unused == 0) && (op.type == 0) &&
            (op.loadBit == loadStore_t::EStore) &&
            (op.preIndexing == prePost_t::EPre) && (op.constant1 == 1) &&
            (op.constant2 == 1);
    }

    template<uint32_t opcode_iter>
    static void execute(Registers& regs, const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        constexpr auto c_op = fromOpcode(opcode_iter);
        constexpr auto storeOperation = memStoreOp<c_op>();
        regs[15] += 4;
        uint32_t offset = regs[op.regOrOffset];

        if constexpr (c_op.addOffset == upDown_t::ESubstract) {
            offset = -offset;
        }
        regs[15] += 4;
        const auto val_to_write = regs[op.destinationRegister];
        uint32_t calculated = regs[op.baseRegister];
        calculated += offset;
        if (op.baseRegister == 15) {
            calculated += 4;
        }

        regs[15] -= 8;
        if constexpr (c_op.writeBack == writeBack_t::EWriteback) {
            regs[op.baseRegister] = calculated;
        }

        storeOperation(calculated, val_to_write);
        // destinationRegisterBug(op, regs);
    }
};

// ----------------
// POST FUNCS STORE
// ---------------

class HalfDataTransferRPoS
{
public:

    static constexpr bool isThisOpcode(const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        return (op.unused == 0) && (op.type == 0) &&
            (op.loadBit == loadStore_t::EStore) &&
            (op.preIndexing == prePost_t::EPost) && (op.constant1 == 1) &&
            (op.constant2 == 1);
    }

    template<uint32_t opcode_iter>
    static void execute(Registers& regs, const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        constexpr auto c_op = fromOpcode(opcode_iter);
        constexpr auto storeOperation = memStoreOp<c_op>();
        regs[15] += 4;
        uint32_t offset = regs[op.regOrOffset];
        if constexpr (c_op.addOffset == upDown_t::ESubstract) {
            offset = -offset;
        }

        uint32_t calculated = regs[op.baseRegister];

        regs[15] += 4;
        const auto value_to_write = regs[op.destinationRegister];
        storeOperation(calculated, value_to_write);
        if (op.baseRegister == 15) {
            calculated += 4;
        }
        calculated += offset;
        regs[15] -= 8;
        regs[op.baseRegister] = calculated;
    }
};

// ----------------
// PRE FUNCS LOAD
// ---------------

class HalfDataTransferRPrL
{
public:

    static constexpr bool isThisOpcode(const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        return (op.unused == 0) && (op.type == 0) &&
            (op.loadBit == loadStore_t::ELoad) &&
            (op.preIndexing == prePost_t::EPre) && (op.constant1 == 1) &&
            (op.constant2 == 1);
    }

    template<uint32_t opcode_iter>
    static void execute(Registers& regs, const uint32_t opcode)
    {
        regs[15] += 4;
        const auto op = fromOpcode(opcode);
        constexpr auto c_op = fromOpcode(opcode_iter);
        constexpr auto loadOperation = HalfDataTransfer::memLoadOp<c_op>();
        uint32_t offset = regs[op.regOrOffset];

        if constexpr (c_op.addOffset == upDown_t::ESubstract) {
            offset = -offset;
        }

        uint32_t calculated = regs[op.baseRegister];
        regs[15] -= 4;
        calculated += offset;

        if constexpr (c_op.writeBack == writeBack_t::EWriteback) {
            regs[op.baseRegister] = calculated;
        }

        regs[op.destinationRegister] = loadOperation(calculated, false);
        // destinationRegisterBug(op, regs);
    }
};

// ----------------
// POST FUNCS LOAD
// ---------------

class HalfDataTransferRPoL
{
public:

    static constexpr bool isThisOpcode(const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        return (op.unused == 0) && (op.type == 0) &&
            (op.loadBit == loadStore_t::ELoad) &&
            (op.preIndexing == prePost_t::EPost) && (op.constant1 == 1) &&
            (op.constant2 == 1);
    }

    template<uint32_t opcode_iter>
    static void execute(Registers& regs, const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        constexpr auto c_op = fromOpcode(opcode_iter);
        constexpr auto loadOperation = memLoadOp<c_op>();
        regs[15] += 4;
        uint32_t offset = regs[op.regOrOffset];
        if constexpr (c_op.addOffset == upDown_t::ESubstract) {
            offset = -offset;
        }
        const uint32_t load_address = regs[op.baseRegister];
        uint32_t calculated = regs[op.baseRegister];
        if (op.baseRegister == 15) {
            calculated += 8;
        }
        calculated += offset;
        regs[op.baseRegister] = calculated;
        regs[15] -= 4;
        regs[op.destinationRegister] = loadOperation(load_address, false);
    }
};

} // namespace HalfDataTransfer
#endif
