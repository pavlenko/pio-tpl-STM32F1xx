#ifndef __SIGNATURE_H__
#define __SIGNATURE_H__

#include <stddef.h>
#include <stdint.h>

#include <stm32cpp/_common.hpp>

// __STATIC_FORCEINLINE
uint32_t sig_get_flash_size()
{
    return *((uint32_t*) FLASHSIZE_BASE) * 1024;
}

// __STATIC_FORCEINLINE
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

// __STATIC_FORCEINLINE
void sig_get_uid(uint32_t *uid)
{
    *uid++ = *((uint32_t*) (UID_BASE + 0));
    *uid++ = *((uint32_t*) (UID_BASE + 4));
    *uid = *((uint32_t*) (UID_BASE + 8));
}

#endif // __SIGNATURE_H__