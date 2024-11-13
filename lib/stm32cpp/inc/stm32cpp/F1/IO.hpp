#ifndef __STM32_IO_F1_HPP__
#define __STM32_IO_F1_HPP__

#if defined(STM32F1)

#include <stm32cpp/common/Clock.hpp>
#include <stm32cpp/common/IO.hpp>

namespace STM32
{
    namespace IO
    {
        using PA = IOPort<Clock::ClockControl<&RCC_TypeDef::APB2ENR, RCC_APB2ENR_IOPAEN>>;
        IO_PORT_DEFINITION(PA, Port::A, GPIOA_BASE);

        using PB = IOPort<Clock::ClockControl<&RCC_TypeDef::APB2ENR, RCC_APB2ENR_IOPBEN>>;
        IO_PORT_DEFINITION(PB, Port::B, GPIOB_BASE);

        using PC = IOPort<Clock::ClockControl<&RCC_TypeDef::APB2ENR, RCC_APB2ENR_IOPCEN>>;
        IO_PORT_DEFINITION(PC, Port::C, GPIOC_BASE);

#if defined(GPIOD_BASE)
        using PD = IOPort<Clock::ClockControl<&RCC_TypeDef::APB2ENR, RCC_APB2ENR_IOPDEN>>;
        IO_PORT_DEFINITION(PD, Port::D, GPIOD_BASE);
#endif
#if defined(GPIOE_BASE)
        using PE = IOPort<Clock::ClockControl<&RCC_TypeDef::APB2ENR, RCC_APB2ENR_IOPEEN>>;
        IO_PORT_DEFINITION(PE, Port::E, GPIOE_BASE);
#endif
#if defined(GPIOF_BASE)
        using PF = IOPort<Clock::ClockControl<&RCC_TypeDef::APB2ENR, RCC_APB2ENR_IOPFEN>>;
        IO_PORT_DEFINITION(PF, Port::F, GPIOF_BASE);
#endif
#if defined(GPIOG_BASE)
        using PG = IOPort<Clock::ClockControl<&RCC_TypeDef::APB2ENR, RCC_APB2ENR_IOPGEN>>;
        IO_PORT_DEFINITION(PG, Port::G, GPIOG_BASE);
#endif
    }
}

#endif

#endif // __STM32_IO_F1_HPP__
