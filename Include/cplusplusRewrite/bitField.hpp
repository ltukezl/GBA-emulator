#pragma once

#include <cassert>
#include <cstdint>
#include <ppltasks.h>

template<class T, size_t start, size_t len>
struct Bits
{
    static constexpr uint32_t mask = (1ULL << len) - 1ULL;
    constexpr auto& get() { return (static_cast<const T&>(*this)); }
    void operator=(const uint32_t other)
    {
        uint32_t masked = (other & mask) << start;
        get().all &= ~(mask << start);
        get().all |= masked;
    }
    constexpr operator uint32_t() { return (get().all >> start) & mask; }
    constexpr operator int32_t() { return (get().all >> start) & mask; }
    constexpr operator uint32_t() const { return (get().all >> start) & mask; }
    constexpr operator int32_t() const { return (get().all >> start) & mask; }

    constexpr bool operator==(const uint32_t other) const
    {
        return ((get().all >> start) & mask) == (static_cast<uint32_t>(other));
    }
};
