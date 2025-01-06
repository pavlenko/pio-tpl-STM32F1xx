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

    template <typename tPort, uint8_t tNumber>
    class Pin
    {
    public:
        static inline void configure();
    };

    template <typename tClock>
    class Port
    {
    private:
        static constexpr GPIO_TypeDef* regs();

    public:
        static inline void enable();
        static inline void disable();
    };
}
