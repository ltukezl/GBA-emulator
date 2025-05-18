#include <bit>

#include "cplusplusRewrite/BarrelShifterDecoder.h"
#include "cplusplusRewrite/barrelShifter.h"


decltype(&ImmediateRotater::calculate) BarrelShifterDecoder::decode(const uint32_t opCode){
	const auto decoder = std::bit_cast<DecodeBits>(opCode);
	if (decoder.immediate)
		return &(ImmediateRotater::calculate);
}
