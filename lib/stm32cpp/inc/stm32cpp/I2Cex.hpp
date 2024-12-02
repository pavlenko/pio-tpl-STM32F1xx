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

    struct Device
    {
        const uint8_t address;
        const Speed speed;
    };

    template <uint32_t RegsT, IRQn_Type EventIRQn, IRQn_Type ErrorIRQn, class ClockT, class DMAtxT, class DMArxT>
    class Driver
    {
        static void enable();
        static void disable();
        static void configure(Speed speed);
        static bool busy();
        static void dispatchEventIRQ();
        static void dispatchErrorIRQ();
    };

    template <class DriverT>
    class Slave
    {
        static void listen(uint8_t address, AddrCallback cb);
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

    template <class DriverT>
    class Memory
    {
        template <typename T>
        static void set(Device &dev, T address, uint8_t *data, size_t size);

        template <typename T>
        static void get(Device &dev, T address, uint8_t *data, size_t size);
    };
}
