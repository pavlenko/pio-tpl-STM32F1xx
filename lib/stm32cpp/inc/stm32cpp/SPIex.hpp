#pragma once

#include <stddef.h>
#include <stdint.h>

// TODO need better API... (multi-slave bus)
// TODO need to store config?
// TODO need example of usages for both modes
namespace STM32::SPIex
{
    class Device
    {
        // Configure bus
        void configure();

        // Listen to CS pin changes
        void listen(uint8_t);

        // Set TX data
        void send(uint8_t *data, size_t size);

        // Set RX data
        void recv(uint8_t *data, size_t size);

        // Set TX/RX data
        void exchange(uint8_t *txData, uint8_t *rxData, size_t size);

        // Dispatch IRQ related to device
        void dispatchIRQ(void);
    };

    // All communications like: select -> transfer -> deselect
    class Master
    {
        // Send sata to device
        void send(Device &dev, uint8_t *data, size_t size);

        // Receive data from device
        void recv(Device &dev, uint8_t *data, size_t size);

        // Exchange data with device
        void exchange(Device &dev, uint8_t *txData, uint8_t *rxData, size_t size);

        // Dispatch IRQ related to master
        void dispatchIRQ(void);
    };
}
