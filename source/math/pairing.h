#pragma once

#include <cstdint>

namespace LotusMath
{
    inline uint64_t szudzikPair(uint32_t x, uint32_t y)
    {
        return (x >= y ? (x * x) + x + y : (y * y) + x);
    }
}