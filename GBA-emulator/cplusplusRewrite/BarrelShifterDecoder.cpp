#include <bit>

#include "cplusplusRewrite/BarrelShifterDecoder.h"
#include "cplusplusRewrite/barrelShifter.h"
#include "cplusplusRewrite/HwRegisters.h"


decltype(&ImmediateRotater::calculate) BarrelShifterDecoder::decode(const uint32_t opCode){
	const auto decoder = std::bit_cast<DecodeBits>(opCode);
	if (decoder.opCode == 0)
	{
		if (decoder.immediate)
			return &(ImmediateRotater::calculate);
		else if (decoder.immediate == 0 && decoder.type == 1)
			return &(RegisterWithRegisterShifter::calculate);
		else
			return &(RegisterWithImmediateShifter::calculate);
	}
	else if (decoder.opCode == 1)
	{
		if (decoder.immediate == 0)
			return &(ImmediateRotater::calculate);
		else if (decoder.immediate == 0 && decoder.type == 1)
			return &(RegisterWithRegisterShifter::calculate);
		else
			return &(RegisterWithImmediateShifter::calculate);
	}

}
