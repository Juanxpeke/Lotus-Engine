#pragma once

#include "types.h"

namespace Lotus
{
    inline uint64_t szudzikPair(uint32_t x, uint32_t y)
    {
        return (x >= y ? (x * x) + x + y : (y * y) + x);
    }
}