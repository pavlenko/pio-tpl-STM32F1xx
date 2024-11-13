#ifndef __STM32_IO_COMMON_HPP__
#define __STM32_IO_COMMON_HPP__

#include <stm32cpp/_common.hpp>
#include <stm32cpp/Clock.hpp>
#include <stm32cpp/EXTI.hpp>

namespace STM32
{
    namespace IO
    {
        /**
         * PORTS:
         * A: F0, F1, F2, F3, F4, F7, G0, G4, L0, L1, L4, L5
         * B: F0, F1, F2, F3, F4, F7, G0, G4, L0, L1, L4, L5
         * C: F0, F1, F2, F3, F4, F7, G0, G4, L0, L1, L4, L5
         * D: F0, F1, F2, F3, F4, F7, G0, G4, L0, L1, L4, L5
         * E: F0, F1, F2, F3, F4, F7, G0, G4, L0, L1, L4, L5
         * F: F0, F1, F2, F3, F4, F7, G0, G4, L0, L1, L4, L5
         * G: F1, F2, F3, F4, F7, G4, L0, L1, L4, L5
         * H: F2, F3, F4, F7, L0, L1, L4, L5
         * I: F2, F4, F7, L4
         * J: F4, F7
         * I: F4, F7
         * K: F4, F7
         */
        enum class Port
        {
            A = 0x0U,
            B = 0x1U,
            C = 0x2U,
#if defined(GPIOD_BASE)
            D = 0x3U,
#endif
#if defined(GPIOE_BASE)
            E = 0x4U,
#endif
#if defined(GPIOF_BASE)
            F = 0x5U,
#endif
#if defined(GPIOG_BASE)
            G = 0x6U,
#endif
#if defined(GPIOH_BASE)
            H = 0x7U,
#endif
#if defined(GPIOI_BASE)
            I = 0x8U,
#endif
#if defined(GPIOJ_BASE)
            J = 0x9U,
#endif
#if defined(GPIOK_BASE)
            K = 0xAU,
#endif
        };

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
#if defined(STM32F1)
            OD = 0b0100,
#else
            OD,
#endif
        };

        enum class Pull
        {
            NO_PULL,
            PULL_UP,
            PULL_DOWN,
        };

        enum class Speed
        {
#if defined(STM32F1)
            LOW = 0b0010,
            MEDIUM = 0b0001,
            FAST = 0b0011,
#else
            LOW,
            MEDIUM,
            FAST,
            HIGH,
#endif
        };

        template <class TClock>
        class IOPort
        {
        public:
            static inline void enable()
            {
                TClock::enable();
            }
            static inline void disable()
            {
                TClock::disable();
            }
        };

        namespace
        {
            template <Port tPort, uint32_t tAddress, uint8_t tNumber>
            class IOPin
            {
            private:
                static_assert(tNumber < 16u, "Invalid pin number");

                static constexpr const uint8_t _2bit_pos = tNumber * 2u;
                static constexpr const uint8_t _4bit_pos = (tNumber & 0x7u) * 4u;

                static inline GPIO_TypeDef* _regs()
                {
                    return reinterpret_cast<GPIO_TypeDef*>(tAddress);
                }

            public:
                static constexpr const Port port = tPort;
                static constexpr const uint8_t num = tNumber;

                template <Mode tMode, typename = typename std::enable_if_t<tMode == Mode::OUTPUT || tMode == Mode::ALTERNATE>>
                static inline void configure(Speed speed, OType otype = OType::PP, uint8_t af = 0)
                {
#if defined(STM32F1)
                    if constexpr (tNumber < 8u)
                    {
                        _regs()->CRL &= ~(0xFu << _4bit_pos);
                        if constexpr (tMode == Mode::ALTERNATE) {
                            _regs()->CRL |= ((0x8u | static_cast<uint8_t>(speed) | static_cast<uint8_t>(otype)) << _4bit_pos);
                        }
                        else {
                            _regs()->CRL |= ((static_cast<uint8_t>(speed) | static_cast<uint8_t>(otype)) << _4bit_pos);
                        }
                    }
                    else
                    {
                        _regs()->CRH &= ~(0xFu << _4bit_pos);
                        if constexpr (tMode == Mode::ALTERNATE) {
                            _regs()->CRH |= ((0x8u | static_cast<uint8_t>(speed) | static_cast<uint8_t>(otype)) << _4bit_pos);
                        }
                        else {
                            _regs()->CRH |= ((static_cast<uint8_t>(speed) | static_cast<uint8_t>(otype)) << _4bit_pos);
                        }
                    }
#else
                    _regs()->OSPEEDR &= ~(0x3u << _2bit_pos);
                    _regs()->OSPEEDR |= (static_cast<uint8_t>(speed) << _2bit_pos);

                    _regs()->OTYPER &= ~(1u << tNumber);
                    _regs()->OTYPER != (static_cast<uint8_t>(otype) << tNumber);

                    if (tMode = Mode::ALTERNATE)
                    {
                        if constexpr (tNumber < 8)
                        {
                            _regs()->AFR[0] &= ~(0xFu << _4bit_pos);
                            _regs()->AFR[0] |= (af << _4bit_pos);
                        }
                        else
                        {
                            _regs()->AFR[1] &= ~(0xFu << _4bit_pos);
                            _regs()->AFR[1] |= (af << _4bit_pos);
                        }
                    }

                    _regs()->MODER &= ~(0x3u << _2bit_pos);
                    _regs()->MODER |= static_cast<uint8_t>(tMode) << _2bit_pos;
#endif
                }

                template <Mode tMode, typename = typename std::enable_if_t<tMode == Mode::INPUT>>
                static inline void configure(Pull pull = Pull::NO_PULL)
                {
#if defined(STM32F1)
                    if constexpr (tNumber < 8u)
                    {
                        _regs()->CRL &= ~(0xFu << _4bit_pos);
                        if (pull == Pull::NO_PULL)
                            _regs()->CRL |= (0x4u << _4bit_pos);
                        else
                            _regs()->CRL |= (0x8u << _4bit_pos);
                    }
                    else
                    {
                        _regs()->CRH &= ~(0xFu << _4bit_pos);
                        if (pull == Pull::NO_PULL)
                            _regs()->CRH |= (0x4u << _4bit_pos);
                        else
                            _regs()->CRH |= (0x8u << _4bit_pos);
                    }
                    if (pull == Pull::PULL_UP)
                        _regs()->BSRR |= (1u << (tNumber));
                    else if (pull == Pull::PULL_DOWN)
                        _regs()->BRR |= (1u << (tNumber));
#else
                    _regs()->PUPDR &= ~(0x3u << _2bit_pos);
                    _regs()->PUPDR |= static_cast<uint8_t>(pull) << _2bit_pos;

                    _regs()->MODER &= ~(0x3u << _2bit_pos);
                    _regs()->MODER |= static_cast<uint8_t>(tMode) << _2bit_pos;
#endif
                }

                template <Mode tMode, typename = typename std::enable_if_t<tMode == Mode::ANALOG>>
                static inline void configure(void)
                {
#if defined(STM32F1)
                    if constexpr (tNumber < 8u)
                        _regs()->CRL &= ~(0xFu << _4bit_pos);
                    else
                        _regs()->CRH &= ~(0xFu << _4bit_pos);
#else
                    _regs()->MODER &= ~(0x3u << _2bit_pos);
#endif
                }

                static inline bool get()
                {
                    return _regs()->IDR & (1u << tNumber);
                }

                static inline void set()
                {
                    _regs()->BSRR |= (1u << tNumber);
                }

                static inline void clr()
                {
                    _regs()->BSRR |= ((1u << tNumber) << 16u);
                }

                static inline void tog()
                {
                    _regs()->ODR ^= (1u << tNumber);
                }

                template <EXTIEdge tEdge>
                static inline void attachIRQ(const EXTICallback callback)
                {
                    EXTIController::instance()
                        .attach<IOPin<tPort, tAddress, tNumber>, tEdge>(callback);
                }

                static inline void detachIRQ()
                {
                    EXTIController::instance()
                        .detach<IOPin<tPort, tAddress, tNumber>>();
                }
            };
        }

#define IO_PORT_DEFINITION(__ALIAS__, __PORT__, __REG_BASE__) \
    using __ALIAS__##0 = IOPin<__PORT__, __REG_BASE__, 0>;    \
    using __ALIAS__##1 = IOPin<__PORT__, __REG_BASE__, 1>;    \
    using __ALIAS__##2 = IOPin<__PORT__, __REG_BASE__, 2>;    \
    using __ALIAS__##3 = IOPin<__PORT__, __REG_BASE__, 3>;    \
    using __ALIAS__##4 = IOPin<__PORT__, __REG_BASE__, 4>;    \
    using __ALIAS__##5 = IOPin<__PORT__, __REG_BASE__, 5>;    \
    using __ALIAS__##6 = IOPin<__PORT__, __REG_BASE__, 6>;    \
    using __ALIAS__##7 = IOPin<__PORT__, __REG_BASE__, 7>;    \
    using __ALIAS__##8 = IOPin<__PORT__, __REG_BASE__, 8>;    \
    using __ALIAS__##9 = IOPin<__PORT__, __REG_BASE__, 9>;    \
    using __ALIAS__##10 = IOPin<__PORT__, __REG_BASE__, 10>;  \
    using __ALIAS__##11 = IOPin<__PORT__, __REG_BASE__, 11>;  \
    using __ALIAS__##12 = IOPin<__PORT__, __REG_BASE__, 12>;  \
    using __ALIAS__##13 = IOPin<__PORT__, __REG_BASE__, 13>;  \
    using __ALIAS__##14 = IOPin<__PORT__, __REG_BASE__, 14>;  \
    using __ALIAS__##15 = IOPin<__PORT__, __REG_BASE__, 15>;

    }
}

#endif // __STM32_IO_COMMON_HPP__
