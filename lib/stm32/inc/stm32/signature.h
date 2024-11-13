#ifndef __STM32_SIGNATIRE_H__
#define __STM32_SIGNATIRE_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STM32F1)
#include <stm32f1xx.h>
#elif defined(STM32F4)
#include <stm32f4xx.h>
#else
#error "stm32 family not defined."
#endif

#include <stddef.h>
#include <stdint.h>

__STATIC_FORCEINLINE
uint32_t sig_get_flash_size()
{
    return *((uint32_t*) FLASHSIZE_BASE) * 1024;
}

__STATIC_FORCEINLINE
uint32_t sig_get_firmware_size(uint32_t offset)
{
    // Resolve end address of flash
    uint32_t addr = FLASH_BASE + (sig_get_flash_size() - 1);

    // Loop through unprogrammed addresses in back order
    while (*((uint32_t*) addr) == 0xFFFFFFFF) {
        addr--;
    }

    // Calculate size in bytes except offset (for bootloaders)
    return (FLASH_BASE + offset) - addr;
}

__STATIC_FORCEINLINE
void sig_get_uid(uint32_t *uid)
{
    *uid++ = *((uint32_t*) (UID_BASE + 0));
    *uid++ = *((uint32_t*) (UID_BASE + 4));
    *uid = *((uint32_t*) (UID_BASE + 8));
}

#ifdef __cplusplus
}
#endif

#endif // __STM32_SIGNATIRE_H__