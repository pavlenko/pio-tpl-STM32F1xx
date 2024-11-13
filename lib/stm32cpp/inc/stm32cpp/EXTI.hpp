#ifndef __STM32_EXTI_HPP__
#define __STM32_EXTI_HPP__

#include <functional>
#include <stdint.h>
#include <stm32cpp/_common.hpp>

namespace STM32
{
    typedef void (*EXTICallback)();

    enum class EXTIEdge
    {
        RISING,
        FALLING,
        BOTH,
    };

    class EXTIController
    {
    private:
        EXTIController()
        {
#if defined(STM32F1)
            __HAL_RCC_AFIO_CLK_ENABLE();
#else
            __HAL_RCC_SYSCFG_CLK_ENABLE();
#endif
        }

        EXTIController(const EXTIController &) = delete; // Prevent copy constructor
        EXTIController(EXTIController &&) = delete;      // Prevent move constructor
        void operator=(const EXTIController &) = delete; // Prevent assignment
        void operator=(EXTIController &&) = delete;      // Prevent reference

        static constexpr size_t numChannels = 16;

        static constexpr IRQn_Type _getIRQn(const uint8_t pin)
        {
            IRQn_Type extiIRQn = static_cast<IRQn_Type>(-100);
            if (pin == 0)
            {
                extiIRQn = EXTI0_IRQn;
            }
            else if (pin == 1)
            {
                extiIRQn = EXTI1_IRQn;
            }
            else if (pin == 2)
            {
                extiIRQn = EXTI2_IRQn;
            }
            else if (pin == 3)
            {
                extiIRQn = EXTI3_IRQn;
            }
            else if (pin == 4)
            {
                extiIRQn = EXTI4_IRQn;
            }
            else if (pin >= 5 && pin <= 9)
            {
                extiIRQn = EXTI9_5_IRQn;
            }
            else if (pin >= 10 && pin <= 15)
            {
                extiIRQn = EXTI15_10_IRQn;
            }
            return extiIRQn;
        }

    public:
        inline static EXTICallback callbacks[numChannels];

        static EXTIController &instance()
        {
            static EXTIController instance;
            return instance;
        }

        template <typename tPin, EXTIEdge edge>
        void attach(const EXTICallback callback)
        {
            constexpr uint8_t port = static_cast<uint8_t>(tPin::port);
            constexpr uint8_t pin = tPin::num;
            constexpr uint8_t reg = (pin / 4u);
            constexpr uint8_t pos = (4u * (pin % 4u));

#if defined(STM32F1)
            AFIO->EXTICR[reg] &= ~(0xFu << pos);
            AFIO->EXTICR[reg] |= (port << pos);
#else
            SYSCFG->EXTICR[reg] &= ~(0xFu << pos);
            SYSCFG->EXTICR[reg] |= (port << pos);
#endif

            if constexpr (edge == EXTIEdge::RISING || edge == EXTIEdge::BOTH)
            {
                EXTI->RTSR |= (1u << pin);
            }
            if constexpr (edge == EXTIEdge::FALLING || edge == EXTIEdge::BOTH)
            {
                EXTI->FTSR |= (1u << pin);
            }

            EXTI->IMR |= (1u << pin);
            callbacks[pin] = callback;

            constexpr IRQn_Type extiIRQn = _getIRQn(pin);
            if (!NVIC_GetEnableIRQ(extiIRQn))
            {
                NVIC_ClearPendingIRQ(extiIRQn);
                NVIC_EnableIRQ(extiIRQn);
            }
        }

        template <typename tPin>
        void detach()
        {
            constexpr uint8_t pin = tPin::num;
            constexpr uint8_t reg = (pin / 4u);
            constexpr uint8_t pos = (4u * (pin % 4u));

#if defined(STM32F1)
            AFIO->EXTICR[reg] &= ~(0xFu << pos);
#else
            SYSCFG->EXTICR[reg] &= ~(0xFu << pos);
#endif

            EXTI->IMR &= ~(1u << pin);
            callbacks[pin] = nullptr;

            bool otherIRQActive = false;
            if constexpr (pin >= 5u && pin <= 9u)
            {
                otherIRQActive = EXTI->IMR & (0x1Fu << 5u);
            }
            else if constexpr (pin >= 10u && pin <= 15u)
            {
                otherIRQActive = EXTI->IMR & (0x3Fu << 10u);
            }
            if (!otherIRQActive)
            {
                constexpr IRQn_Type extiIRQn = _getIRQn(pin);
                NVIC_DisableIRQ(extiIRQn);
            }
        }

        void dispatchIRQ(const uint8_t pin)
        {
            if (EXTI->PR & (1u << pin))
            {
                EXTI->PR |= (1u << pin);
                if (callbacks[pin])
                    callbacks[pin](); //<-- for detect edge just read pin value, if 0 - falling, else rising
            }
        }
    };
}

#endif // __STM32_EXTI_HPP__