#ifndef __STM32_FLASH_F4_HPP__
#define __STM32_FLASH_F4_HPP__

#if defined(STM32F4)

#include <stm32cpp/common/Flash.hpp>

namespace STM32
{
    constexpr uint32_t Flash::getPageSize(uint16_t page)
    {
#if defined(FLASH_BANK_2)
        // bank 1
        if (page < 4)
            return 0x4000;
        if (page == 4)
            return 0x10000;
        // bank 2
        if (page < 16)
            return 0x4000;
        if (page == 16)
            return 0x10000;
        return 0x20000;
#else
        if (page < 4)
            return 0x4000;
        if (page == 4)
            return 0x10000;
        return 0x20000;
#endif
    }

    constexpr uint32_t Flash::getPageAddress(uint16_t page)
    {
#if defined(FLASH_BANK_2)
        // bank 1
        if (page < 4)
            return FLASH_BASE + (page * 0x4000);
        if (page == 4)
            return FLASH_BASE + 0x10000;
        if (page < 12)
            return FLASH_BASE + 0x20000 + ((page - 5) * 0x20000);
        // bank 2
        if (page < 16)
            return FLASH_BASE + 0x100000 + ((page - 15) * 0x4000);
        if (page == 16)
            return FLASH_BASE + 0x110000;
        return FLASH_BASE + 0x120000 + ((page - 17) * 0x20000);
#else
        if (page < 4)
            return FLASH_BASE + (page * 0x4000);
        if (page == 4)
            return FLASH_BASE + 0x10000;
        return FLASH_BASE + 0x20000 + ((page - 5) * 0x20000);
#endif
    }

    void Flash::erasePage(uint16_t page)
    {
        while ((FLASH->SR & FLASH_SR_BSY) != 0u);

        if (page >= 12) {
            page += 4;
        }

        FLASH->CR = (FLASH->CR & ~FLASH_CR_SNB) | (page << FLASH_CR_SNB_Pos);
        FLASH->CR |= FLASH_CR_SER;
        FLASH->CR |= FLASH_CR_STRT;

        while ((FLASH->SR & FLASH_SR_BSY) != 0u);

        FLASH->CR &= ~(FLASH_CR_SER | FLASH_CR_SNB);
    }

    void Flash::write(uint32_t address, uint8_t data)
    {
        while ((FLASH->SR & FLASH_SR_BSY) != 0u);

        FLASH->CR &= ~FLASH_CR_PSIZE;
        FLASH->CR |= 0u << FLASH_CR_PSIZE_Pos;
        FLASH->CR |= FLASH_CR_PG;

        *(volatile uint8_t*)address = data;

        while ((FLASH->SR & FLASH_SR_BSY) != 0u);
    }

    void Flash::write(uint32_t address, uint16_t data)
    {
        while ((FLASH->SR & FLASH_SR_BSY) != 0u);

        FLASH->CR &= ~FLASH_CR_PSIZE;
        FLASH->CR |= 1u << FLASH_CR_PSIZE_Pos;
        FLASH->CR |= FLASH_CR_PG;

        *(volatile uint16_t*)address = data;

        while ((FLASH->SR & FLASH_SR_BSY) != 0u);
    }

    void Flash::write(uint32_t address, uint32_t data)
    {
        while ((FLASH->SR & FLASH_SR_BSY) != 0u);

        FLASH->CR &= ~FLASH_CR_PSIZE;
        FLASH->CR |= 2u << FLASH_CR_PSIZE_Pos;
        FLASH->CR |= FLASH_CR_PG;

        *(volatile uint32_t*)address = data;

        while ((FLASH->SR & FLASH_SR_BSY) != 0u);
    }

    void Flash::write(uint32_t address, uint64_t data)
    {
        while ((FLASH->SR & FLASH_SR_BSY) != 0u);

        FLASH->CR &= ~FLASH_CR_PSIZE;
        FLASH->CR |= 3u << FLASH_CR_PSIZE_Pos;
        FLASH->CR |= FLASH_CR_PG;

        *(volatile uint32_t*)address = (uint32_t)data;
        __ISB();
        *(volatile uint32_t*)(address + 4) = (uint32_t)(data >> 32);

        while ((FLASH->SR & FLASH_SR_BSY) != 0u);
    }

    template <Flash::WriteSize tSize>
    void Flash::write(uint32_t address, uint8_t* data, size_t size)
    {
        if constexpr (tSize = Flash::WriteSize::X08) {
            for (size_t i = 0; i < size; i += sizeof(uint8_t)) {
                write(address + i, *(uint8_t*)data);
            }
        }

        if constexpr (tSize = Flash::WriteSize::X16) {
            for (size_t i = 0; i < size; i += sizeof(uint16_t)) {
                write(address + i, *(uint64_t*)data);
            }
        }

        if constexpr (tSize = Flash::WriteSize::X32) {
            for (size_t i = 0; i < size; i += sizeof(uint32_t)) {
                write(address + i, *(uint32_t*)data);
            }
        }

        if constexpr (tSize = Flash::WriteSize::X64) {
            for (size_t i = 0; i < size; i += sizeof(uint64_t)) {
                write(address + i, *(uint64_t*)data);
            }
        }
    }
}

#endif

#endif // __STM32_FLASH_F4_HPP__
