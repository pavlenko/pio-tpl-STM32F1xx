#ifndef __STM32_SIGNATURE_H__
#define __STM32_SIGNATURE_H__

#include <cstddef>
#include <cstdint>

#include <stm32cpp/_common.hpp>

namespace STM32
{
    namespace Signature
    {
        static inline uint32_t getFlashSize()
        {
            return *((uint32_t *)FLASHSIZE_BASE) * 1024;
        }

        static inline uint32_t getProgramSize(uint32_t offset)
        {
            uint32_t addr = FLASH_BASE + (getFlashSize() - 1);

            while (*((uint32_t *)addr) == 0xFFFFFFFF)
                addr--;

            return (FLASH_BASE + offset) - addr;
        }

        static inline void getUID(uint32_t uid[3])
        {
            *uid++ = *((uint32_t *)(UID_BASE + 0));
            *uid++ = *((uint32_t *)(UID_BASE + 4));
            *uid = *((uint32_t *)(UID_BASE + 8));
        }
    };
}

#endif // __STM32_SIGNATURE_H__