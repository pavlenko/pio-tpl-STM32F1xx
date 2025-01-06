#pragma once

namespace STM32::IO
{
    enum class Mode
    {
        INPUT,
        OUTPUT,
        ALTERNATE,
        ANALOG,
    };

    enum class OType
    {
        PP,
        OD,
    };

    enum class Pull
    {
        NO_PULL,
        PULL_UP,
        PULL_DOWN,
    };

    enum class Speed
    {
        LOW,
        MEDIUM,
        FAST,
        FASTEST,
    };

    /**
     * @brief IO pin API
     */
    template <typename tPort, uint8_t tNumber>
    class Pin
    {
    public:
        /**
         * @brief Configure pin mode, pull, speed...
         */
        static inline void configure();

        /**
         * @brief Set pi AF number (if supported)
         */
        static inline void setAlternate(uint8_t number);

        /**
         * @brief Get pin value
         */
        static inline bool get();

        /**
         * @brief Set pin to 1
         */
        static inline void set();

        /**
         * @brief Set pin to 0
         */
        static inline void clr();

        /**
         * @brief Toggle pin state
         */
        static inline void tog();
    };

    /**
     * @brief IO port API
     */
    template <typename tClock>
    class Port
    {
    private:
        /**
         * @brief Get port regs ptr
         */
        static constexpr GPIO_TypeDef* regs();

    public:
        /**
         * @brief Enable port clock
         */
        static inline void enable();

        /**
         * @brief Disable port clock
         */
        static inline void disable();
    };
}
