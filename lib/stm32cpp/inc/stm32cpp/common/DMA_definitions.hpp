#pragma once

#include <stdint.h>

#include <stm32cpp/_common.hpp>

namespace STM32::DMA
{
    enum class Config
    {
#ifdef DMA_CCR_EN
        // Direction
        PER_2_MEM = 0x00000000u,
        MEM_2_PER = DMA_CCR_DIR,
        MEM_2_MEM = DMA_CCR_MEM2MEM,
        // Circular mode
        CIRCULAR = DMA_CCR_CIRC,
        // Increments
        PINC = DMA_CCR_PINC,
        MINC = DMA_CCR_MINC,
        // Periph data size
        PSIZE_8BIT = 0x00000000u,
        PSIZE_16BIT = DMA_CCR_PSIZE_0,
        PSIZE_32BIT = DMA_CCR_PSIZE_1,
        // Memory data size
        MSIZE_8BIT = 0x00000000u,
        MSIZE_16BIT = DMA_CCR_MSIZE_0,
        MSIZE_32BIT = DMA_CCR_MSIZE_1,
        // Priority
        PRIORITY_LOW = 0x00000000u,
        PRIORITY_MEDIUM = DMA_CCR_PL_0,
        PRIORITY_HIGH = DMA_CCR_PL_1,
        PRIORITY_VERY_HIGH = DMA_CCR_PL,
        // Interrupts
        IRQ_TRANSFER_ERROR = DMA_CCR_TEIE,
        IRQ_TRANSFER_COMPLETE = DMA_CCR_TCIE,
        IRQ_HALF_TRANSFER = DMA_CCR_HTIE,
#endif
    };

    inline constexpr Config operator|(Config lft, Config rgt)
    {
        return Config(static_cast<uint32_t>(lft) | static_cast<uint32_t>(rgt));
    }

    enum class Flag : uint8_t
    {
#ifdef DMA_CCR_EN
        GLOBAL = DMA_IFCR_CGIF1,
        TRANSFER_COMPLETE = DMA_IFCR_CTCIF1,
        HALF_TRANSFER = DMA_IFCR_CHTIF1,
        TRANSFER_ERROR = DMA_IFCR_CTEIF1,
        ALL = GLOBAL | TRANSFER_COMPLETE | HALF_TRANSFER | TRANSFER_ERROR,
#endif
#ifdef DMA_SxCR_EN
        TRANSFER_COMPLETE = DMA_LISR_TCIF0,
        HALF_TRANSFER = DMA_LISR_HTIF0,
        TRANSFER_ERROR = DMA_LISR_TEIF0,
        FIFO_ERROR = DMA_LISR_FEIF0,
        DIRECT_ERROR = DMA_LISR_DMEIF0,
        ALL = TRANSFER_COMPLETE | HALF_TRANSFER | TRANSFER_ERROR | FIFO_ERROR | DIRECT_ERROR,
#endif
    };

    struct Data
    {
        TransferCallback callback{nullptr};
        void *data{nullptr};
        uint16_t size{0};

        inline void notify(bool success)
        {
            if (callback)
                callback(data, size, success);
        }
    };

}
