#ifndef __STM32_FLASH_F1_HPP__
#define __STM32_FLASH_F1_HPP__

#if defined(STM32F1)

#include <stm32cpp/common/Flash.hpp>

#define FLASH_SIZE 0

namespace STM32
{
    constexpr uint32_t Flash::getPageSize(uint16_t page)
    {
        // x4 - 16kb; x6 - 32kb; x8 - 64kb; xB - 128kb; xC - 256kb; xD - 384kb; xE - 512kb; xF - 768kb; xG - 1M
#if defined(STM32F101xE)\
    || defined(STM32F101xG)\
    || defined(STM32F103xE)\
    || defined(STM32F103xG)\
    || defined(STM32F105xC)\
    || defined(STM32F107xC)
        return 0x800;
#else
        return 0x400;
#endif
    }

    constexpr uint32_t Flash::getPageAddress(uint16_t page)
    {
        return FLASH_BASE + (page * getPageSize(page));
    }

    void Flash::erasePage(uint16_t page)
    {
        uint32_t address = getPageAddress(page);

#if defined(FLASH_BANK_2)
        if (address > FLASH_BANK1_END) {
            while ((FLASH->SR2 & FLASH_SR2_BSY) != 0u);

            FLASH->CR2 |= FLASH_CR2_PER;
            FLASH->AR2 = address;
            FLASH->CR2 |= FLASH_CR2_STRT;

            while ((FLASH->SR2 & FLASH_SR2_BSY) != 0u);

            FLASH->CR2 &= ~FLASH_CR2_PER;
            return;
        }
#endif
        while ((FLASH->SR & FLASH_SR_BSY) != 0u);

        FLASH->CR |= FLASH_CR_PER;
        FLASH->AR = address;
        FLASH->CR |= FLASH_CR_STRT;

        while ((FLASH->SR & FLASH_SR_BSY) != 0u);

        FLASH->CR &= ~FLASH_CR_PER;
    }

    void Flash::write(uint32_t address, uint16_t data)
    {
#if defined(FLASH_BANK_2)
        if (address > FLASH_BANK1_END) {
            while ((FLASH->SR2 & FLASH_SR2_BSY) != 0u);

            FLASH->CR2 |= FLASH_CR2_PG;

            *(volatile uint16_t*)address = data;

            while ((FLASH->SR2 & FLASH_SR2_BSY) != 0u);

            FLASH->CR2 &= ~FLASH_CR2_PG;
        }
#endif
        while ((FLASH->SR & FLASH_SR_BSY) != 0u);

        FLASH->CR |= FLASH_CR_PG;

        *(volatile uint16_t*)address = data;

        while ((FLASH->SR & FLASH_SR_BSY) != 0u);

        FLASH->CR &= ~FLASH_CR_PG;
    }

    void Flash::write(uint32_t address, uint32_t data)
    {
        write(address, (uint16_t)data);
        write(address + 2u, (uint16_t)(data >> 16u));
    }

    void Flash::write(uint32_t address, uint64_t data)
    {
        write(address, (uint16_t)data);
        write(address + 2u, (uint16_t)(data >> 16u));
        write(address + 4u, (uint16_t)(data >> 32u));
        write(address + 6u, (uint16_t)(data >> 48u));
    }

    template <Flash::WriteSize tSize>
    void Flash::write(uint32_t address, uint8_t* data, size_t size)
    {
        for (size_t i = 0; i < size; i += 2) {
            write(address + i, *(uint16_t*)data);
        }
    }
}

#endif

#endif // __STM32_FLASH_F1_HPP__
