#pragma once

#include <stddef.h>
#include <stdint.h>
#include <type_traits>

#include <stm32cpp/_common.hpp>
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
        // enable SPI, enable IRQ vector
        static void enable(void);
        // disable SPI, disable IRQ vector
        static void disable(void);
        // slave: configure bus
        static void configure(Config config);
        // master: configure bus, set SS pin for use
        static void select(Config config);
        // slave: listen for SS pin changes
        template <class PinT>
        static void listen(void);
        // both: send data
        static void send(uint8_t *data, uint16_t size);
        // both: read data
        static void recv(uint8_t *data, uint16_t size);
        // master:
        static void memSet(uint32_t reg, uint8_t *data, uint16_t size);
        // master:
        static void memGet(uint32_t reg, uint8_t *data, uint16_t size);
        // check ready -> get state
        static bool busy();
        static void dispatchIRQ();
    };
}
