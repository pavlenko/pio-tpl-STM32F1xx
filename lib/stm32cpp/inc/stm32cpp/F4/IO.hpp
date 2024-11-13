#ifndef __STM32_IO_F4_HPP__
#define __STM32_IO_F4_HPP__

#if defined(STM32F4)

#include <stm32cpp/common/Clock.hpp>
#include <stm32cpp/common/IO.hpp>

namespace STM32
{
    namespace IO
    {
        using PA = IOPort<Clock::ClockControl<&RCC_TypeDef::AHB1ENR, RCC_AHB1ENR_GPIOAEN>>;
        IO_PORT_DEFINITION(PA, Port::A, GPIOA_BASE);

        using PB = IOPort<Clock::ClockControl<&RCC_TypeDef::AHB1ENR, RCC_AHB1ENR_GPIOBEN>>;
        IO_PORT_DEFINITION(PB, Port::B, GPIOB_BASE);

        using PC = IOPort<Clock::ClockControl<&RCC_TypeDef::AHB1ENR, RCC_AHB1ENR_GPIOCEN>>;
        IO_PORT_DEFINITION(PC, Port::C, GPIOC_BASE);

#if defined(GPIOD_BASE)
        using PD = IOPort<Clock::ClockControl<&RCC_TypeDef::AHB1ENR, RCC_AHB1ENR_GPIODEN>>;
        IO_PORT_DEFINITION(PD, Port::D, GPIOD_BASE);
#endif
#if defined(GPIOE_BASE)
        using PE = IOPort<Clock::ClockControl<&RCC_TypeDef::AHB1ENR, RCC_AHB1ENR_GPIOEEN>>;
        IO_PORT_DEFINITION(PE, Port::E, GPIOE_BASE);
#endif
#if defined(GPIOF_BASE)
        using PF = IOPort<Clock::ClockControl<&RCC_TypeDef::AHB1ENR, RCC_AHB1ENR_GPIOFEN>>;
        IO_PORT_DEFINITION(PF, Port::F, GPIOF_BASE);
#endif
#if defined(GPIOG_BASE)
        using PG = IOPort<Clock::ClockControl<&RCC_TypeDef::AHB1ENR, RCC_AHB1ENR_GPIOGEN>>;
        IO_PORT_DEFINITION(PG, Port::G, GPIOG_BASE);
#endif
#if defined(GPIOH_BASE)
        using PH = IOPort<Clock::ClockControl<&RCC_TypeDef::AHB1ENR, RCC_AHB1ENR_GPIOHEN>>;
        IO_PORT_DEFINITION(PH, Port::H, GPIOH_BASE);
#endif
#if defined(GPIOI_BASE)
        using PI = IOPort<Clock::ClockControl<&RCC_TypeDef::AHB1ENR, RCC_AHB1ENR_GPIOIEN>>;
        IO_PORT_DEFINITION(PI, Port::I, GPIOI_BASE);
#endif
#if defined(GPIOJ_BASE)
        using PJ = IOPort<Clock::ClockControl<&RCC_TypeDef::AHB1ENR, RCC_AHB1ENR_GPIOJEN>>;
        IO_PORT_DEFINITION(PJ, Port::J, GPIOJ_BASE);
#endif
#if defined(GPIOK_BASE)
        using PK = IOPort<Clock::ClockControl<&RCC_TypeDef::AHB1ENR, RCC_AHB1ENR_GPIOKEN>>;
        IO_PORT_DEFINITION(PK, Port::K, GPIOK_BASE);
#endif
    }
}

#endif

#endif // __STM32_IO_F4_HPP__
