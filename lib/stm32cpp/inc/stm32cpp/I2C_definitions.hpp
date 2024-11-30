#pragma once

#include <stdint.h>

namespace STM32::I2C
{
    enum class Direction
    {
        RX,
        TX,
    };

    enum class Speed : uint32_t
    {
        STANDARD = 100000,
        FAST = 400000,
        FAST_PLUS = 1000000,
    };

}
