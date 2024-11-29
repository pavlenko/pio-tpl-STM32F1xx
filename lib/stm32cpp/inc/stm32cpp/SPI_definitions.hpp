#ifndef __STM32_SPI_DEFINITIONS_HPP__
#define __STM32_SPI_DEFINITIONS_HPP__

namespace STM32::SPI
{

    enum class Mode
    {
        SLAVE,
        MASTER,
    };

    enum class BusLine
    {
        TWO_LINE,
        TWO_LINE_RXONLY,
        ONE_LINE_BIDIR,
    };

    enum class BusMode
    {
        MODE_0, // CPOL = 0, CPHA = 0
        MODE_1, // CPOL = 0, CPHA = 1
        MODE_2, // CPOL = 1, CPHA = 0
        MODE_3, // CPOL = 1, CPHA = 1
    };

    enum class BitOrder
    {
        MSB,
        LSB,
    };
}

#endif // __STM32_SPI_DEFINITIONS_HPP__