#ifndef __STM32_F1_FLASH_H__
#define __STM32_F1_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stm32f1xx.h>
#include <stddef.h>
#include <stdint.h>

#define FLASH_LATENCY_0WS 0x00
#define FLASH_LATENCY_1WS 0x01
#define FLASH_LATENCY_2WS 0x02

__STATIC_FORCEINLINE
void flash_prefetch_enable(void)
{
    FLASH->ACR |= FLASH_ACR_PRFTBE;
}

__STATIC_FORCEINLINE
void flash_prefetch_disable(void)
{
    FLASH->ACR &= ~FLASH_ACR_PRFTBE;
}

__STATIC_FORCEINLINE
void flash_set_ws(uint32_t ws)
{
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | (ws << FLASH_ACR_LATENCY_Pos);
}

__STATIC_FORCEINLINE
void flash_unlock_option_bytes(void)
{
    FLASH->OPTKEYR = FLASH_OPTKEY1;
    FLASH->OPTKEYR = FLASH_OPTKEY2;
}

__STATIC_FORCEINLINE
void flash_lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

__STATIC_FORCEINLINE
void flash_unlock(void)
{
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
}

void flash_program_half_word(uint32_t address, uint16_t data);
void flash_erase_page(uint32_t page_address);

#ifdef __cplusplus
}
#endif

#endif // __STM32_F1_FLASH_H__