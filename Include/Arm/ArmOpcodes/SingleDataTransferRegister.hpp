#ifndef SDTR_H
#define SDTR_H
#include <cstdint>

#include "cplusplusRewrite/BarrelShifterDecoder.h"
#include "cplusplusRewrite/HwRegisters.h"
#include "SingleDataTransfer.hpp"

namespace SingleDataTransfer {

// ----------
// PRE FUNCS STORE
// ----------

class SingleDataTransferRPrS
{
public:

    static constexpr bool isThisOpcode(const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) &&
            (op.preIndexing == prePost_t::EPre) &&
            (op.immediateOffset == immediate_t::EBarrelShifter);
    }

    template<uint32_t opcode_iter>
    static void execute(Registers& regs, const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        constexpr auto c_op = fromOpcode(opcode_iter);
        constexpr auto storeOperation = memStoreOp<c_op>();
        const auto func = BarrelShifterDecoder::decode(opcode);
        uint32_t offset = func(regs, opcode, 0);
        uint32_t calculated = regs[op.baseRegister];
        auto val_to_write = regs[op.destinationRegister];
        if (op.baseRegister == 15) {
            calculated += 4;
        }
        if (op.destinationRegister == 15) {
            val_to_write += 8;
        }

        if constexpr (c_op.addOffset == upDown_t::ESubstract) {
            offset = -offset;
        }

        calculated += offset;

        if constexpr (c_op.writeBack == writeBack_t::EWriteback) {
            if (op.baseRegister == 15) {
                calculated += 4;
            }
            regs[op.baseRegister] = calculated;
        }

        storeOperation(calculated, val_to_write);
    }
};

// ----------------
// POST FUNCS STORE
// ---------------

class SingleDataTransferRPoS
{
public:

    static constexpr bool isThisOpcode(const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        return (op.unused == 1) && (op.loadBit == loadStore_t::EStore) &&
            (op.preIndexing == prePost_t::EPost) &&
            (op.immediateOffset == immediate_t::EBarrelShifter);
    }

    template<uint32_t opcode_iter>
    static void execute(Registers& regs, const uint32_t opcode)
    {
        const auto op = fromOpcode(opcode);
        constexpr auto c_op = fromOpcode(opcode_iter);
        constexpr auto storeOperation = memStoreOp<c_op>();
        const auto func = BarrelShifterDecoder::decode(opcode);
        uint32_t offset = func(regs, opcode, 0);
        uint32_t calculated = regs[op.baseRegister];
        uint32_t value_to_write = regs[op.destinationRegister];

        if (op.baseRegister == 15) {
            calculated += 4;
        }
        if (op.destinationRegister == 15) {
            value_to_write += 8;
        }

        if constexpr (c_op.addOffset == upDown_t::ESubstract) {
            offset = -offset;
        }

        storeOperation(calculated, value_to_write);
        calculated += offset;
        if (op.baseRegister == 15) {
            calculated += 4;
        }
        regs[op.baseRegister] = calculated;
    }
};

// ----------------
// PRE FUNCS LOAD
// ---------------

class SingleDataTransferRPrL
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
        uint32_t calculated = regs[op.baseRegister];

        if (op.baseRegister == 15) {
            calculated += 4;
        }

        if constexpr (c_op.addOffset == upDown_t::ESubstract) {
            offset = -offset;
        }

        const uint32_t load_address = calculated + offset;
        calculated += offset;

        if constexpr (c_op.writeBack == writeBack_t::EWriteback) {
            if (op.baseRegister == 15) {
                calculated += 4;
            }
            regs[op.baseRegister] = calculated;
        }

        uint32_t loaded_value = loadOperation(load_address, false);

        regs[op.destinationRegister] = loaded_value;
    }
};

// ----------------
// POST FUNCS LOAD
// ---------------

class SingleDataTransferRPoL
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
        uint32_t calculated = regs[op.baseRegister];

        if (op.baseRegister == 15) {
            calculated += 4;
        }

        if constexpr (c_op.addOffset == upDown_t::ESubstract) {
            offset = -offset;
        }
        const uint32_t load_address = calculated;

        calculated += offset;
        if (op.baseRegister == 15) {
            calculated += 4;
        }
        regs[op.baseRegister] = calculated;

        const auto ret = loadOperation(load_address, false);
        regs[op.destinationRegister] = ret;
    }
};
} // namespace SingleDataTransfer

#endif
