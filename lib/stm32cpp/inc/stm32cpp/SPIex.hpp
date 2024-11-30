#pragma once

#include <stddef.h>
#include <stdint.h>

#include <stm32cpp/_common.hpp>
#include <stm32cpp/IO.hpp>
#include <stm32cpp/SPI_definitions.hpp>

using namespace STM32::SPI;

namespace STM32::SPIex
{
    typedef void (*Callback)(size_t size);

    struct Config
    {
        const uint32_t speed;
        const BusLine bl;
        const BusMode bm;
        const BitOrder bo;
    };

    // Specific SPIn driver
    class Driver
    {
    public:
        void configure(Config c);
        void send(uint8_t *data, size_t size);
        void recv(uint8_t *data, size_t size);
        void exchange(uint8_t *txData, uint8_t *rxData, size_t size);
        void dispatchIRQ();
    };

    class Slave
    {
    public:
        Slave(Driver &drv, Config c, uint8_t pin);
    };

    struct Device
    {
        uint8_t pin;
        Config conf;
    };

    class Master
    {
    public:
        Master(Driver &drv);
        void send(Device &dev, uint8_t *data, size_t size);
        void recv(Device &dev, uint8_t *data, size_t size);
        void exchange(Device &dev, uint8_t *txData, uint8_t *rxData, size_t size);
    };
}

void example1()
{
    using namespace STM32;

    IO::PA0 pin;
    SPIex::Config cfg{1000};
    SPIex::Driver spi;

    uint8_t data[] = "TEST";

    spi.configure(cfg);
    pin.clr();
    spi.send(data, sizeof(data));
    pin.set();
}

void example2()
{
    using namespace STM32;

    SPIex::Device dev{0, {1000}};
    SPIex::Driver drv;
    SPIex::Master spi{drv};

    uint8_t data[] = "TEST";

    spi.send(dev, data, sizeof(data));
}
