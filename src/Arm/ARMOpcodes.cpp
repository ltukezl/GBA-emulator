#include <array>
#include <cstdint>
#include <string>
#include <utility>

#include "Arm/armopcodes.h"
#include "Arm/ArmOpcodes/BlockDataTransferLoads.hpp"
#include "Arm/ArmOpcodes/BlockDataTransferStores.hpp"
#include "Arm/ArmOpcodes/Branch.hpp"
#include "Arm/ArmOpcodes/DataProcessing.hpp"
#include "Arm/ArmOpcodes/Multiply.hpp"
#include "Arm/ArmOpcodes/SDDHelper.hpp"
#include "Arm/ArmOpcodes/Undefop.hpp"
#include "CommonOperations/conditions.h"
#include "cplusplusRewrite/HwRegisters.h"
#include "Display/Disassembler.hpp"
#include "GBAcpu.h"
#include "Include/Arm/ArmOpcodes/SingleDataTransfer.hpp"
#include "Interrupt/interrupt.h"
#include "Memory/memoryOps.h"

uint32_t RORnoCond(uint32_t immediate, uint32_t by)
{
    if (by > 32) {
        RORnoCond(immediate, by - 32);
    }
    return (immediate >> by) | (immediate << (32 - by));
}

static void null_func(Registers&, const uint32_t) {}

static uint32_t constexpr reduce_opcode(const uint32_t opCode)
{
    const uint32_t low_byte = (opCode >> 4) & 0xF;
    const uint32_t high_byte = (opCode >> 16) & 0xFF0;
    return low_byte | high_byte;
}

template<uint32_t opCode>
static auto constexpr decode_arm_opcode()
{
    if constexpr (UndefOp::isThisOpcode(opCode)) {
        return &UndefOp::execute;
    }
    if constexpr (DataProcessing::isThisOpcode(opCode)) {
        return &DataProcessing::execute<DataProcessing::mask(opCode)>;
    }

    if constexpr (SingleDataTransfer::isThisOpcode(opCode)) {
        return SingleDataTransfer::decode_sdd<opCode>();
    }
    if constexpr (BlockDataTransfer::isThisOpcode(opCode)) {
        if constexpr (BlockDataTransferLoad::isThisOpcode(opCode)) {
            return BlockDataTransferLoad::execute<BlockDataTransfer::mask(
                opCode)>;
        }
        if constexpr (BlockDataTransferStore::isThisOpcode(opCode)) {
            return BlockDataTransferStore::execute<BlockDataTransfer::mask(
                opCode)>;
        }
    }

    if constexpr (branches::ArmBranch::isThisOpcode(opCode)) {
        return branches::ArmBranch::execute<branches::ArmBranch::mask(opCode)>;
    }

    if constexpr (MultiplyAccumulate::isThisOpcode(opCode)) {
        return &MultiplyAccumulate::execute<MultiplyAccumulate::mask(opCode)>;
    }

    if constexpr (MultiplyLong::isThisOpcode(opCode)) {
        return &MultiplyLong::execute<MultiplyLong::mask(opCode)>;
    }

    // -- decoding correct this much
    if constexpr (((opCode >> 26) & 0x3) == 0) {
        return HalfDataTransfer::decode_hdd<opCode>();
    }

    return &null_func;
}

static consteval auto index_to_opcode(const uint32_t opcode)
{
    const uint32_t low_bits = opcode & 0xF;
    const uint32_t high_bits = opcode & 0xFF0;
    return (high_bits << 16) | (low_bits << 4);
}

template<typename T, std::size_t... Is>
consteval void insert_opcodes(T& arr, std::index_sequence<Is...>)
{ ((arr[Is] = decode_arm_opcode<index_to_opcode(Is)>()), ...); }

static constexpr std::array m_dispatch_table = {[]() consteval {
    std::array<void (*)(Registers&, const uint32_t), 0xC00> tmp{};
    insert_opcodes(tmp, std::make_index_sequence<tmp.size()>{});
    return tmp;
}()};

void ARMExecute(int opCode)
{
    // runSingleStepTests_a();
    // return;

    int condition = (opCode >> 28) & 0xF;
    cycles += 1;

    if (conditions[condition](r)) // condition true
    {
        int opCodeType = (opCode >> 24) & 0xF;
        switch (opCodeType) {
            case 15: interruptController(); break;
            default: m_dispatch_table[reduce_opcode(opCode)](r, opCode); break;
        }
    }
}
