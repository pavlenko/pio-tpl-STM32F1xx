#pragma once

#include <type_traits>
#include <stddef.h>
#include <stdint.h>

extern "C"
{
#include <stm32f1xx_hal_i2c.h>
}

#include <stm32cpp/I2C_definitions.hpp>

using namespace STM32::I2C;

namespace STM32::I2Cex
{
    using AddrCallback = std::add_pointer_t<void(Direction dir)>;
    using DoneCallback = std::add_pointer_t<void(size_t size)>;

    template <uint32_t RegsAddrT, IRQn_Type EventIRQn, IRQn_Type ErrorIRQn, class ClockT>
    class Driver
    {
        static void enable();

        static void disable();

        static void configure(Speed speed);
    };

    template <class DriverT>
    class Slave
    {
        static void configure(Speed speed);

        static void listen(uint8_t address, AddrCallback cb);

        static void send(uint8_t *data, size_t size, DoneCallback cb = nullptr);

        static void recv(uint8_t *data, size_t size, DoneCallback cb = nullptr);

        static void dispatchIRQ();
    };

    struct Device
    {
        const uint8_t address;
        const Speed speed;
    };

    template <class DriverT>
    class Master
    {
    public:
        static void send(Device &dev, uint8_t *data, size_t size, DoneCallback cb = nullptr);

        static void recv(Device &dev, uint8_t *data, size_t size, DoneCallback cb = nullptr);

        template <typename T>
        static void memSet(Device &dev, T address, uint8_t *data, size_t size, DoneCallback cb = nullptr);

        template <typename T>
        static void memGet(Device &dev, T address, uint8_t *data, size_t size, DoneCallback cb = nullptr);

        static void dispatchIRQ();
    };
}
