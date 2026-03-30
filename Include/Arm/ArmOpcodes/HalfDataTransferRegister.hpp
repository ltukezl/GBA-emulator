#ifndef HDTR_H
#define HDTR_H
#include <cstdint>

#include "cplusplusRewrite/BarrelShifterDecoder.h"
#include "cplusplusRewrite/HwRegisters.h"
#include "HalfDataTransfer.hpp"

namespace HalfDataTransfer {

// ----------
// PRE FUNCS STORE
// ----------

class HalfDataTransferPrS
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
        uint32_t offset = regs[c_op.regOrOffset];

        if constexpr (c_op.addOffset == upDown_t::ESubstract) {
            offset = -offset;
        }

        uint32_t calculated = regs[op.baseRegister];
        calculated += offset;

        if constexpr (c_op.writeBack == writeBack_t::EWriteback) {
            regs[op.baseRegister] = calculated;
        }

        storeOperation(calculated, regs[op.destinationRegister]);
        destinationRegisterBug(op, regs);
    }
};

// ----------------
// POST FUNCS STORE
// ---------------

class HalfDataTransferPoS
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
        uint32_t offset = regs[c_op.regOrOffset];
        if constexpr (c_op.addOffset == upDown_t::ESubstract) {
            offset = -offset;
        }

        uint32_t calculated = regs[op.baseRegister];
        storeOperation(calculated, regs[op.destinationRegister]);
        calculated += offset;
        regs[op.baseRegister] = calculated;
        destinationRegisterBug(op, regs);
    }
};

// ----------------
// PRE FUNCS LOAD
// ---------------
/*
class HalfDataTransferRPrL
{
public:

    static constexpr bool isThisOpcode(const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) &&
            (op.preIndexing == prePost_t::EPre) &&
            (op.immediateOffset == immediate_t::EBarrelShifter);
    }

    template<uint32_t opcode_iter>
    static void execute(Registers& regs, const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        constexpr auto c_op = fromOpcode(opcode_iter);
        constexpr auto loadOperation = memLoadOp<c_op>();
        const auto func = BarrelShifterDecoder::decode(opcode);
        uint32_t offset = func(regs, opcode, 0);
        if constexpr (c_op.addOffset == upDown_t::ESubstract) {
            offset = -offset;
        }

        uint32_t calculated = regs[op.baseRegister];
        calculated += offset;

        if constexpr (c_op.writeBack == writeBack_t::EWriteback) {
            regs[op.baseRegister] = calculated;
        }

        regs[op.destinationRegister] = loadOperation(calculated, false);
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
        return (op.unused == 1) && (op.loadBit == loadStore_t::ELoad) &&
            (op.preIndexing == prePost_t::EPost) &&
            (op.immediateOffset == immediate_t::EBarrelShifter);
    }

    template<uint32_t opcode_iter>
    static void execute(Registers& regs, const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        constexpr auto c_op = fromOpcode(opcode_iter);
        constexpr auto loadOperation = memLoadOp<c_op>();
        const auto func = BarrelShifterDecoder::decode(opcode);
        uint32_t offset = func(regs, opcode, 0);
        if constexpr (c_op.addOffset == upDown_t::ESubstract) {
            offset = -offset;
        }

        uint32_t calculated = regs[op.baseRegister];
        const auto ret = loadOperation(calculated, false);
        calculated += offset;
        regs[op.baseRegister] = calculated;
        regs[op.destinationRegister] = ret;
        destinationRegisterBug(op, regs);
    }
};
*/
} // namespace HalfDataTransfer
#endif
