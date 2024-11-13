#ifndef __STM32_IO_H__
#define __STM32_IO_H__

#include <stdint.h>

#include <stm32f1xx.h>

/* Platform dependent implementations */
//TODO mode
//TODO maybe do not create IO abstraction due mcu has different registers for that?
enum class IOMode {
    INPUT,
    OUTPUT_PP,
    OUTPUT_OD,
    ANALOG,
};

enum class IOPull {
    NO_PULL,
    PULL_UP,
    PULL_DOWN,
};

enum class IOSpeed {
    LOW,
    MEDIUM,
    FAST,
    HIGH, //<-- except F1
};

//TODO periph pin definitions with remap
//TODO configure, irq for list of pins

template<uint32_t address>
struct IO {
    static constexpr const uint32_t _address = address;

    static inline void configure(IOPull pull, IOSpeed speed, uint16_t pins)
    {
        // F1 only
        reinterpret_cast<GPIO_TypeDef *>(_address)->CRL |= GPIO_CRL_MODE0_1;// in/out speed
        reinterpret_cast<GPIO_TypeDef *>(_address)->CRL |= GPIO_CRL_CNF0_1;// in/out; pullup/down; analog

        // F4
        // separate registers...
    }

    static inline uint16_t get(uint16_t mask)
    {
        return reinterpret_cast<GPIO_TypeDef *>(_address)->IDR & mask;
    }

    static inline void set(uint16_t mask)
    {
        reinterpret_cast<GPIO_TypeDef *>(_address)->BSRR |= mask;
        //asm volatile("dsb;");
    }

    static inline void clr(uint16_t mask)
    {
        reinterpret_cast<GPIO_TypeDef *>(_address)->BRR &= mask;
        //asm volatile("dsb;");
    }

    static inline void tog(uint16_t mask)
    {
        reinterpret_cast<GPIO_TypeDef *>(_address)->ODR ^= mask;
        //asm volatile("dsb;");
    }
};

#endif // __STM32_IO_H__