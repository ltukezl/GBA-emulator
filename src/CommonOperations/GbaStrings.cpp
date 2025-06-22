#include <format>

#include "CommonOperations/GbaStrings.hpp"


std::string create_range_string(const uint32_t s, const uint32_t e)
{
	return std::format("R{}-R{}", s, e);
}

std::string create_plain_reg(const uint32_t s)
{
	return std::format("R{}", s);
}

std::string create_rlist(const uint32_t rlist, const uint32_t regs)
{
	std::string baseString = "";
	for (size_t i = 0; i < regs;)
	{
		if (!(rlist & (1 << i)))
		{
			++i;
			continue;
		}

		// Found the start of a run
		size_t start = i;
		while (i < regs && (rlist & (1 << i)))
			++i;

		// Generate string based on run length
		if (!baseString.empty())
			baseString += ",";

		if (start == i - 1)
			baseString += create_plain_reg(start);
		else
			baseString += create_range_string(start, i - 1);
	}
	return baseString;
}
