#ifndef __STM32_IO_F0_HPP__
#define __STM32_IO_F0_HPP__

#if defined(STM32F0)

#include <stm32cpp/common/Clock.hpp>
#include <stm32cpp/common/IO.hpp>

namespace STM32
{
    namespace IO
    {
        using PA = IOPort<Clock::ClockControl<&RCC_TypeDef::AHBENR, RCC_AHBENR_GPIOAEN>>;
        IO_PORT_DEFINITION(PA, Port::A, GPIOA_BASE);

        using PB = IOPort<Clock::ClockControl<&RCC_TypeDef::AHBENR, RCC_AHBENR_GPIOBEN>>;
        IO_PORT_DEFINITION(PB, Port::B, GPIOB_BASE);

        using PC = IOPort<Clock::ClockControl<&RCC_TypeDef::AHBENR, RCC_AHBENR_GPIOCEN>>;
        IO_PORT_DEFINITION(PC, Port::C, GPIOC_BASE);
#if defined(GPIOD_BASE)
        using PD = IOPort<Clock::ClockControl<&RCC_TypeDef::AHBENR, RCC_AHBENR_GPIODEN>>;
        IO_PORT_DEFINITION(PD, Port::D, GPIOD_BASE);
#endif
#if defined(GPIOE_BASE)
        using PE = IOPort<Clock::ClockControl<&RCC_TypeDef::AHBENR, RCC_AHBENR_GPIOEEN>>;
        IO_PORT_DEFINITION(PE, Port::E, GPIOE_BASE);
#endif
#if defined(GPIOF_BASE)
        using PF = IOPort<Clock::ClockControl<&RCC_TypeDef::AHBENR, RCC_AHBENR_GPIOFEN>>;
        IO_PORT_DEFINITION(PF, Port::F, GPIOF_BASE);
#endif
    }
}

#endif

#endif // __STM32_IO_F0_HPP__
