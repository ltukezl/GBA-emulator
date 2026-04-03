#ifndef SDDHELPER_H
#define SDDHELPER_H

#include <cassert>

#include "Arm/ArmOpcodes/HalfDataTransferImmediate.hpp"
#include "Arm/ArmOpcodes/HalfDataTransferRegister.hpp"
#include "Arm/ArmOpcodes/SingleDataTransferImmediate.hpp"
#include "Arm/ArmOpcodes/SingleDataTransferRegister.hpp"

namespace SingleDataTransfer {
static void null_func_sdd(Registers&, const uint32_t) {}

template<uint32_t opCode>
static consteval decltype(&SingleDataTransferIPrS::execute<0>) decode_sdd()
{
    if constexpr (SingleDataTransferIPrS::isThisOpcode(opCode)) {
        return &SingleDataTransferIPrS::execute<mask(opCode)>;
    } else if constexpr (SingleDataTransferIPrL::isThisOpcode(opCode)) {
        return &SingleDataTransferIPrL::execute<mask(opCode)>;
    } else if constexpr (SingleDataTransferIPoS::isThisOpcode(opCode)) {
        return &SingleDataTransferIPoS::execute<mask(opCode)>;
    } else if constexpr (SingleDataTransferIPoL::isThisOpcode(opCode)) {
        return &SingleDataTransferIPoL::execute<mask(opCode)>;
    }
    if constexpr (SingleDataTransferRPrS::isThisOpcode(opCode)) {
        return &SingleDataTransferRPrS::execute<mask(opCode)>;
    } else if constexpr (SingleDataTransferRPrL::isThisOpcode(opCode)) {
        return &SingleDataTransferRPrL::execute<mask(opCode)>;
    } else if constexpr (SingleDataTransferRPoS::isThisOpcode(opCode)) {
        return &SingleDataTransferRPoS::execute<mask(opCode)>;
    } else if constexpr (SingleDataTransferRPoL::isThisOpcode(opCode)) {
        return &SingleDataTransferRPoL::execute<mask(opCode)>;
    } else {
        return &null_func_sdd;
    }
}
} // namespace SingleDataTransfer

namespace HalfDataTransfer {
static void null_func_hdd(Registers&, const uint32_t) {}

template<uint32_t opCode>
static consteval decltype(&HalfDataTransferRPrS::execute<0>) decode_hdd()
{

    if constexpr (HalfDataTransferIPrS::isThisOpcode(opCode)) {
        return &HalfDataTransferIPrS::execute<mask(opCode)>;
    } else if constexpr (HalfDataTransferIPrL::isThisOpcode(opCode)) {
        return &HalfDataTransferIPrL::execute<mask(opCode)>;
    } else if constexpr (HalfDataTransferIPoS::isThisOpcode(opCode)) {
        return &HalfDataTransferIPoS::execute<mask(opCode)>;
    } else if constexpr (HalfDataTransferIPoL::isThisOpcode(opCode)) {
        return &HalfDataTransferIPoL::execute<mask(opCode)>;
    } else if constexpr (HalfDataTransferRPrS::isThisOpcode(opCode)) {
        return &HalfDataTransferRPrS::execute<mask(opCode)>;
    } else if constexpr (HalfDataTransferRPoS::isThisOpcode(opCode)) {
        return &HalfDataTransferRPoS::execute<mask(opCode)>;
    } else if constexpr (HalfDataTransferRPrL::isThisOpcode(opCode)) {
        return &HalfDataTransferRPrL::execute<mask(opCode)>;
    } else if constexpr (HalfDataTransferRPoL::isThisOpcode(opCode)) {
        return &HalfDataTransferRPoL::execute<mask(opCode)>;
    }

    else {
        return &null_func_hdd;
    }
}
} // namespace HalfDataTransfer

#endif
