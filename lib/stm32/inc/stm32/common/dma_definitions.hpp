#pragma once

namespace STM32::DMA
{
    enum class Config;

    enum class Flags;

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
