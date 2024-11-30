#pragma once

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
    using AddrCallback = void(Direction dir);
    using DoneCallback = void(size_t size);

    // Specific I2Cn driver
    class Driver
    {
        void configure(Speed speed);
    };

    class Slave
    {
        void configure(Speed speed);
        void listen(uint8_t address, AddrCallback cb);
        void send(uint8_t *data, size_t size, DoneCallback cb = nullptr);
        void recv(uint8_t *data, size_t size, DoneCallback cb = nullptr);
        void dispatchIRQ();
    };

    struct Device
    {
        const uint8_t address;
        const Speed speed;
    };

    class Master
    {
        void send(Device &dev, uint8_t *data, size_t size, DoneCallback cb = nullptr);
        void recv(Device &dev, uint8_t *data, size_t size, DoneCallback cb = nullptr);
        template <typename T>
        void memSet(Device &dev, T address, uint8_t *data, size_t size, DoneCallback cb = nullptr);
        template <typename T>
        void memGet(Device &dev, T address, uint8_t *data, size_t size, DoneCallback cb = nullptr);
        void dispatchIRQ();
    };
}
