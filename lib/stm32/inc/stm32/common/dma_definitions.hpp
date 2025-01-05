#pragma once

namespace STM32::DMA
{
    enum class Config;

    enum class Flags;

    /**
     * @brief DMA channel APIs
     */
    template <typename tDriver, uint32_t tRegsAddress, uint32_t tChannel, IRQn_Type tIRQn>
    class Channel 
    {
        /**
         * @brief Transfer data via DMA
         */
        static inline void transfer(Config config, const void* buffer, volatile void* periph, uint32_t size);

        /**
         * @brief Abort DMA transfer
         */
        static inline void abort();
    };

    /**
     * @brief DMA bus APIs
     */
    template <uint32_t tRegsAddress, typename tClock>
    class Driver
    {};
}
