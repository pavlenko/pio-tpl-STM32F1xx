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

    template <uint32_t RegsT, IRQn_Type EventIRQn, IRQn_Type ErrorIRQn, class ClockT, class DMAtxT, class DMArxT>
    class Driver
    {
        static void enable(); // master/slave

        static void disable(); // master/slave

        static void configure(Speed speed); // master/slave

        static void send(uint8_t *data, size_t size); // master/slave

        static void recv(uint8_t *data, size_t size); // master/slave

        static bool busy(); // master/slave

        static void dispatchEventIRQ(); // master/slave

        static void dispatchErrorIRQ(); // master/slave
    };

    template <class DriverT>
    class Slave
    {
        static void listen(uint8_t address, AddrCallback cb);
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
    };
}
