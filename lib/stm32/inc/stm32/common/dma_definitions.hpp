#pragma once

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
#ifdef DMA_SxCR_EN
        // Direction
        PER_2_MEM = 0x00000000,
        MEM_2_PER = DMA_SxCR_DIR_0,
        MEM_2_MEM = DMA_SxCR_DIR_1,
        // Circular mode
        CIRCULAR = DMA_SxCR_CIRC,
        // Increments
        MINC = DMA_SxCR_MINC,
        PINC = DMA_SxCR_PINC,
        // Periph data size
        PSIZE_8BIT = 0x00000000,
        PSIZE_16BIT = DMA_SxCR_PSIZE_0,
        PSIZE_32BIT = DMA_SxCR_PSIZE_1,
        // Memory data size
        MSIZE_8BIT = 0x00000000,
        MSIZE_16BIT = DMA_SxCR_MSIZE_0,
        MSIZE_32BIT = DMA_SxCR_MSIZE_1,
        // Priority
        PRIORITY_LOW = 0x00000000u,
        PRIORITY_MEDIUM = DMA_SxCR_PL_0,
        PRIORITY_HIGH = DMA_SxCR_PL_1,
        PRIORITY_VERY_HIGH = DMA_SxCR_PL_1 | DMA_SxCR_PL_0,
        // Enable IRQ
        IRQ_TRANSFER_ERROR = DMA_SxCR_TEIE,
        IRQ_HALF_TRANSFER = DMA_SxCR_HTIE,
        IRQ_TRANSFER_COMPLETE = DMA_SxCR_TCIE,
        IRQ_DIRECT_MODE_ERROR = DMA_SxCR_DMEIE,
        IRQ_FIFO_ERROR = DMA_SxCR_FIFOIE,
#endif
    };

    enum class Flag
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

    /**
     * @brief DMA channel APIs
     *
     * @tparam tDriver      Bus driver
     * @tparam tRegsAddress Base address of channel registers
     * @tparam tChannel     Channel number
     * @tparam tIRQn        Channel IRQ number
     */
    template <typename tDriver, uint32_t tRegsAddress, uint32_t tChannel, IRQn_Type tIRQn>
    class Channel 
    {
    public:
        /**
         * @brief Transfer data via DMA
         *
         * @param config Transfer configuration
         * @param buffer Data buffer ptr
         * @param periph Peripheral buffer address
         * @param size   Transfer size
         */
        static inline void transfer(Config config, const void* buffer, volatile void* periph, uint32_t size);

        /**
         * @brief Abort DMA transfer
         */
        static inline void abort();
    };

    /**
     * @brief DMA bus APIs
     *
     * @tparam tRegsAddress Base address of bus registers
     * @tparam tClock       Bus clock control
     */
    template <uint32_t tRegsAddress, typename tClock>
    class Driver
    {
    private:
        /**
         * @brief Get ptr to DMA registers struct
         *
         * @return Registers struct ptr
         */
        static constexpr DMA_TypeDef *_regs();

    public:
        /**
         * @brief Enable DMA clock
         */
        static inline void enable();

        /**
         * @brief Disable DMA clock
         */
        static inline void disable();

        /**
         * @brief Check channel flag(s) is set
         *
         * @tparam tChannel Channel num
         * @tparam tFlag    Flag(s) to check
         */
        template <uint8_t tChannel, Flag tFlag>
        static inline bool hasChannelFlag();

        /**
         * @brief Clear channel flag(s)
         *
         * @tparam tChannel Channel num
         * @tparam tFlag    Flag(s) to clear
         */
        template <uint8_t tChannel, Flag tFlag>
        static inline void clrChannelFlag();

        /**
         * @brief Clear channel all flags
         *
         * @tparam tChannel Channel num
         */
        template <uint8_t tChannel>
        static inline void clrChannelFlags();
    };
}
