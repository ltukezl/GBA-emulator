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

        uint32_t offset = regs[op.regOrOffset];
        uint32_t calculated = regs[op.baseRegister];
        auto val_to_write = regs[op.destinationRegister];

        if (op.regOrOffset == 15) {
            offset += 4;
        }
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

        storeOperation(calculated, val_to_write);

        if constexpr (c_op.writeBack == writeBack_t::EWriteback) {
            if (op.baseRegister == 15) {
                calculated += 4;
            }
            regs[op.baseRegister] = calculated;
        }
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
        uint32_t offset = regs[op.regOrOffset];
        uint32_t calculated = regs[op.baseRegister];
        uint32_t value_to_write = regs[op.destinationRegister];

        if (op.regOrOffset == 15) {
            offset += 4;
        }
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
        const auto op = fromOpcode(opcode);
        constexpr auto c_op = fromOpcode(opcode_iter);
        constexpr auto loadOperation = HalfDataTransfer::memLoadOp<c_op>();
        uint32_t offset = regs[op.regOrOffset];
        uint32_t calculated = regs[op.baseRegister];

        if (op.regOrOffset == 15) {
            offset += 4;
        }
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
        if constexpr (c_op.signOrUnsign == 1 &&
                      c_op.HForByte == byteWord_t::EByte) {
            if (loaded_value & 0x80) { // sign bit on
                loaded_value |= 0xFFFFFF00;
            }
        } else if constexpr (c_op.signOrUnsign == 1 &&
                             c_op.HForByte == byteWord_t::EHWord) {
            if (load_address & 1) {
                loaded_value &= 0xFFFF;
                if (loaded_value & 0x80) { // sign bit on
                    loaded_value |= 0xFFFFFF00;
                }
            } else {
                if (loaded_value & 0x8000) { // sign bit on
                    loaded_value |= 0xFFFF0000;
                }
            }
        }
        regs[op.destinationRegister] = loaded_value;
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
        uint32_t offset = regs[op.regOrOffset];
        uint32_t calculated = regs[op.baseRegister];

        if (op.regOrOffset == 15) {
            offset += 4;
        }
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
        uint32_t loaded_value = loadOperation(load_address, false);
        if constexpr (c_op.signOrUnsign == 1 &&
                      c_op.HForByte == byteWord_t::EByte) {
            if (loaded_value & 0x80) { // sign bit on
                loaded_value |= 0xFFFFFF00;
            }
        } else if constexpr (c_op.signOrUnsign == 1 &&
                             c_op.HForByte == byteWord_t::EHWord) {
            if (load_address & 1) {
                loaded_value &= 0xFFFF;
                if (loaded_value & 0x80) { // sign bit on
                    loaded_value |= 0xFFFFFF00;
                }
            } else {
                if (loaded_value & 0x8000) { // sign bit on
                    loaded_value |= 0xFFFF0000;
                }
            }
        }
        regs[op.destinationRegister] = loaded_value;
    }
};

} // namespace HalfDataTransfer
#endif
