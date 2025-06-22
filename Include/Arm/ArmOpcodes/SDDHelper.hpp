#ifndef SDDHELPER_H
#define SDDHELPER_H

#include <cassert>

#include "Arm/ArmOpcodes/SingleDataTransferImmediate.hpp"
#include "Arm/ArmOpcodes/SingleDataTransferRegister.hpp"

namespace SingleDataTransfer
{
	static void null_func22(Registers&, const uint32_t) {}

	template<uint32_t opCode>
	static consteval decltype(&SingleDataTransferIPrS::execute<0>) decode_sdd()
	{
		if constexpr (SingleDataTransferIPrS::isThisOpcode(opCode))
		{
			return &SingleDataTransferIPrS::execute<mask(opCode)>;
		}
		else if constexpr (SingleDataTransferIPrL::isThisOpcode(opCode))
		{
			return &SingleDataTransferIPrL::execute<mask(opCode)>;
		}
		else if constexpr (SingleDataTransferIPoS::isThisOpcode(opCode))
		{
			return &SingleDataTransferIPoS::execute<mask(opCode)>;
		}
		else if constexpr (SingleDataTransferIPoL::isThisOpcode(opCode))
		{
			return &SingleDataTransferIPoL::execute<mask(opCode)>;
		}
		if constexpr (SingleDataTransferRPrS::isThisOpcode(opCode))
		{
			return &SingleDataTransferRPrS::execute<mask(opCode)>;
		}
		else if constexpr (SingleDataTransferRPrL::isThisOpcode(opCode))
		{
			return &SingleDataTransferRPrL::execute<mask(opCode)>;
		}
		else if constexpr (SingleDataTransferRPoS::isThisOpcode(opCode))
		{
			return &SingleDataTransferRPoS::execute<mask(opCode)>;
		}
		else if constexpr (SingleDataTransferRPoL::isThisOpcode(opCode))
		{
			return &SingleDataTransferRPoL::execute<mask(opCode)>;
		}
		else
		{
			return &null_func22;
		}
	}
}

#endif