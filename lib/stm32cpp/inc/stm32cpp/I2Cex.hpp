#pragma once

#include <type_traits>
#include <stddef.h>
#include <stdint.h>

#include <stm32cpp/I2C_definitions.hpp>

extern "C"
{
#include <stm32f1xx_hal_i2c.h>
}

using namespace STM32::I2C;

namespace STM32::I2Cex
{
    using AddrCallback = std::add_pointer_t<void(Direction dir)>;
    using DoneCallback = std::add_pointer_t<void(size_t size)>;

    struct Config
    {
        const uint16_t address;
        const Speed speed;
        //add: generall call; no-strech
    };

    template <uint32_t RegsT, IRQn_Type EventIRQn, IRQn_Type ErrorIRQn, class ClockT, class DMAtxT, class DMArxT>
    class Driver
    {
    public:
        // enable I2C; enable IRQ
        static void enable();
        // disable I2C; disable IRQ
        static void disable();
        // slave: set ownAddress; set bus speed
        static void configure(Config &config);
        // slave: enable IRQ flags
        static void listen();
        // master: enable IRQ flags, set bus speed
        static void select(Config &device);
        // slave(master?): send response
        static void send(uint8_t *data, uint16_t size);
        // slave(master?): read incoming data
        static void recv(uint8_t *data, uint16_t size);
        // master
        static void memSet(uint16_t reg, uint8_t *data, uint16_t size);
        // master
        static void memGet(uint16_t reg, uint8_t *data, uint16_t size);
        // check if not ready -> get state
        static bool busy();
        // need separate for master/slave
        static void dispatchEventIRQ();
        static void dispatchErrorIRQ();
    };
}
