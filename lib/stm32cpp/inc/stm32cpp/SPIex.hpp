#pragma once

#include <stddef.h>
#include <stdint.h>
#include <type_traits>

#include <stm32cpp/_common.hpp>
#include <stm32cpp/IO.hpp>
#include <stm32cpp/SPI_definitions.hpp>

extern "C"
{
#include <stm32f1xx_hal_spi.h>
}

using namespace STM32::SPI;

namespace STM32::SPIex
{
    using Callback = std::add_pointer_t<void(size_t size)>;

    struct Config
    {
        const uint32_t speed;
        const BusLine bl;
        const BusMode bm;
        const BitOrder bo;
    };

    template <class PinT>
    struct Device
    {
        const Config conf;
    };

    template <uint32_t RegsT, IRQn_Type IRQnT, class ClockT, class DMAtxT, class DMArxT>
    class Driver
    {
    public:
        static void enable();
        static void disable();
        static void configure(Config config);
        static bool busy();
        static void dispatchIRQ();
    };

    template <class DriverT>
    class Slave
    {
    public:
        template <class PinT>
        static void listen();
        static void send(uint8_t *data, size_t size);
        static void recv(uint8_t *data, size_t size);
    };

    template <class DriverT>
    class Master
    {
    public:
        static void send(Device &dev, uint8_t *data, size_t size);
        static void recv(Device &dev, uint8_t *data, size_t size);
    };
}
