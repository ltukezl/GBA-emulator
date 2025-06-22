#ifndef GBASTRINGS_H
#define GBASTRINGS_H

#include <array>
#include <cstdint>
#include <string>

static constexpr std::array<const char*, 15> condition_strings = { "EQ", "NE", "CS", "CC", "MI", "PL", "VS", "VC", "HI", "LS", "GE", "LT", "GT", "LE", "" };

std::string create_range_string(const uint32_t s, const uint32_t e);

std::string create_plain_reg(const uint32_t s);

std::string create_rlist(const uint32_t rlist, const uint32_t regs);

#endif