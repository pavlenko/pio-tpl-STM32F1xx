#ifndef __STM32_FLASH_COMMON_HPP__
#define __STM32_FLASH_COMMON_HPP__

#include <stm32cpp/_common.hpp>

namespace STM32
{
    class Flash
    {
    public:
        enum class WriteSize
        {
#if defined(STM32F2) || defined(STM32F4)
            X08,
#endif
            X16,
            X32,
            X64,
        };
    private:
        static inline constexpr uint32_t getPageSize(uint16_t page);

        static inline constexpr uint32_t getPageAddress(uint16_t page);

    public:
        static inline uint8_t getLatency()
        {
            return (FLASH->ACR & FLASH_ACR_LATENCY) >> FLASH_ACR_LATENCY_Pos;
        }

        static inline void setLatency(uint8_t clocks)
        {
            MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, clocks << FLASH_ACR_LATENCY_Pos);
        }

        static inline void lock()
        {
            FLASH->CR |= FLASH_CR_LOCK;
        }

        static inline void unlock()
        {
            FLASH->KEYR = FLASH_KEY1;
            FLASH->KEYR = FLASH_KEY2;
        }

        static inline void erasePage(uint16_t page);

#if defined(STM32F2) || defined(STM32F4)
        static inline void write(uint32_t address, uint8_t data);
#endif

        static inline void write(uint32_t address, uint16_t data);

        static inline void write(uint32_t address, uint32_t data);

        static inline void write(uint32_t address, uint64_t data);

        template <WriteSize tSize>
        static inline void write(uint32_t address, uint8_t* data, size_t size);
    };
}

#endif // __STM32_FLASH_COMMON_HPP__
