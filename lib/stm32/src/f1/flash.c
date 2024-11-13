
#include <stm32/f1/flash.h>

void flash_program_half_word(uint32_t address, uint16_t data)
{
    while ((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY);

    FLASH->CR |= FLASH_CR_PG;

    *(__IO uint16_t *)(address) = data;

    while ((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY);

    FLASH->CR &= ~FLASH_CR_PG;
}

void flash_erase_page(uint32_t page_address)
{
    while ((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY);

    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = page_address;
    FLASH->CR |= FLASH_CR_STRT;

    while ((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY);

    FLASH->CR &= ~FLASH_CR_PER;
}