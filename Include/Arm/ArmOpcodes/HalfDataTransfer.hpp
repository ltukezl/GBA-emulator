#ifndef HDT_H
#define HDT_H

#include <cassert>
#include <Constants.h>
#include <cstdint>
#include <format>
#include <type_traits>

#include "cplusplusRewrite/BarrelShifterDecoder.h"
#include "cplusplusRewrite/HwRegisters.h"
#include "Memory/memoryOps.h"

namespace HalfDataTransfer {

enum class loadStore_t : uint32_t
{
    EStore,
    ELoad,
};

enum class writeBack_t : uint32_t
{
    ENoWriteback,
    EWriteback
};

enum class byteWord_t : uint32_t
{
    EHWord,
    EByte
};

enum class upDown_t : uint32_t
{
    ESubstract,
    EAdd
};

enum class prePost_t : uint32_t
{
    EPost,
    EPre,
};

enum class immediate_t : uint32_t
{
    EImmediate,
    EBarrelShifter,
};

struct HalfDataTransfer_t
{
    uint32_t regOrOffset: 4; // barrel shifter
    uint32_t constant1: 1;
    byteWord_t HForByte: 1;
    uint32_t signOrUnsign: 1;
    uint32_t constant2: 5;
    uint32_t destinationRegister: 4;
    uint32_t baseRegister: 4;
    loadStore_t loadBit: 1;
    writeBack_t writeBack: 1;
    uint32_t type: 1;
    upDown_t addOffset: 1;
    prePost_t preIndexing: 1;
    uint32_t unused: 3;
    uint32_t executionCondition: 4;
};

static constexpr HalfDataTransfer_t fromOpcode(const uint32_t opcode)
{
    assert(std::is_trivially_copyable_v<HalfDataTransfer_t>);
    assert(sizeof(HalfDataTransfer_t) == sizeof(uint32_t));
    return {
        .regOrOffset = static_cast<uint32_t>((opcode >> 0x00) & 0xF),
        .constant1 = static_cast<uint32_t>((opcode >> 0x04) & 0x1),
        .HForByte = static_cast<byteWord_t>((opcode >> 0x05) & 0x1),
        .signOrUnsign = static_cast<uint32_t>((opcode >> 0x06) & 0x1),
        .constant2 = static_cast<uint32_t>((opcode >> 0x07) & 0x1F),
        .destinationRegister = static_cast<uint32_t>((opcode >> 0x0C) & 0xF),
        .baseRegister = static_cast<uint32_t>((opcode >> 0x10) & 0xF),
        .loadBit = static_cast<loadStore_t>((opcode >> 0x14) & 0x1),
        .writeBack = static_cast<writeBack_t>((opcode >> 0x15) & 0x1),
        .type = static_cast<uint32_t>((opcode >> 0x16) & 0x1),
        .addOffset = static_cast<upDown_t>((opcode >> 0x17) & 0x1),
        .preIndexing = static_cast<prePost_t>((opcode >> 0x18) & 0x1),
        .unused = static_cast<uint32_t>((opcode >> 0x19) & 0x7),
        .executionCondition = static_cast<uint32_t>((opcode >> 0x1C) & 0xF)};
}

static constexpr auto mask(const uint32_t opcode)
{ return opcode & (0x3F << 20); }

static inline void destinationRegisterBug(const HalfDataTransfer_t& op,
                                          Registers& regs)
{
    if (op.destinationRegister == 15) {
        regs[op.destinationRegister] -= 8;
    }
}

/*
template<HalfDataTransfer_t op>
static consteval auto memLoadOp()
{
    if constexpr (op.byteTransfer == byteWord_t::EWord) {
        return &loadFromAddress32;
    } else {
        return &loadFromAddress;
    }
}
*/
template<HalfDataTransfer_t op>
static consteval auto memStoreOp()
{
    if constexpr (op.HForByte == byteWord_t::EHWord) {
        return &writeToAddress16;
    } else {
        return &writeToAddress;
    }
}

static auto makeExpression(const uint32_t opcode)
{ return ""; }

static auto disassemble(const uint32_t opcode)
{ return ""; }
} // namespace HalfDataTransfer

#endif
