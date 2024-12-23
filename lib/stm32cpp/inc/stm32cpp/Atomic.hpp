#pragma once
#include <core_cm3.h>

class DisableInterrupts
{
public:
    DisableInterrupts()
        : _sreg(__get_PRIMASK())
    {
        __disable_irq();
    }
    ~DisableInterrupts()
    {
        __set_PRIMASK(_sreg);
    }
    operator bool()
    {
        return false;
    }

private:
    uint32_t _sreg;
};

#define ATOMIC                                      \
    if (DisableInterrupts di = DisableInterrupts()) \
    {                                               \
    }                                               \
    else

namespace Private
{
    inline uint8_t LDREX(unsigned char *addr)
    {
        return __LDREXB(addr);
    }

    inline uint8_t LDREX(signed char *addr)
    {
        return __LDREXB((uint8_t *)addr);
    }

    inline uint8_t LDREX(char *addr)
    {
        return __LDREXB((uint8_t *)addr);
    }

    inline uint16_t LDREX(uint16_t *addr)
    {
        return __LDREXH(addr);
    }

    inline uint16_t LDREX(int16_t *addr)
    {
        return __LDREXH((uint16_t *)addr);
    }

    inline uint32_t LDREX(int *addr)
    {
        return __LDREXW((uint32_t *)addr);
    }

    inline uint32_t LDREX(unsigned *addr)
    {
        return __LDREXW((uint32_t *)addr);
    }

    inline uint32_t LDREX(long *addr)
    {
        return __LDREXW((uint32_t *)addr);
    }

    inline uint32_t LDREX(unsigned long *addr)
    {
        return __LDREXW((uint32_t *)addr);
    }

    template <class T>
    inline T *LDREX(T **addr)
    {
        return (T *)__LDREXW((uint32_t *)addr);
    }

    inline uint32_t STREX(unsigned char value, unsigned char *addr)
    {
        return __STREXB((uint8_t)value, (uint8_t *)addr);
    }

    inline uint32_t STREX(signed char value, signed char *addr)
    {
        return __STREXB((uint8_t)value, (uint8_t *)addr);
    }

    inline uint32_t STREX(char value, char *addr)
    {
        return __STREXB((uint8_t)value, (uint8_t *)addr);
    }

    inline uint32_t STREX(uint16_t value, uint16_t *addr)
    {
        return __STREXH(value, addr);
    }

    inline uint32_t STREX(int16_t value, int16_t *addr)
    {
        return __STREXH((uint16_t)value, (uint16_t *)addr);
    }

    inline uint32_t STREX(int value, int *addr)
    {
        return __STREXW((uint32_t)value, (uint32_t *)addr);
    }

    inline uint32_t STREX(unsigned value, unsigned *addr)
    {
        return __STREXW((uint32_t)value, (uint32_t *)addr);
    }

    inline uint32_t STREX(unsigned long value, unsigned long *addr)
    {
        return __STREXW((uint32_t)value, (uint32_t *)addr);
    }

    inline uint32_t STREX(long value, long *addr)
    {
        return __STREXW((uint32_t)value, (uint32_t *)addr);
    }

    template <class T>
    inline uint32_t STREX(T *value, T **addr)
    {
        return __STREXW((uint32_t)value, (uint32_t *)addr);
    }

    template <class T, class T2>
    inline uint32_t STREX(T value, T2 *addr)
    {
        static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4);
        if (sizeof(T) == 1)
            return __STREXB((uint8_t)value, (uint8_t *)addr);
        if (sizeof(T) == 2)
            return __STREXH((uint16_t)value, (uint16_t *)addr);
        return __STREXW((uint32_t)value, (uint32_t *)addr);
    }

    template <class T>
    inline T LDREX(T *addr)
    {
        static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4);
        if (sizeof(T) == 1)
            return (T)__LDREXB((uint8_t *)addr);
        if (sizeof(T) == 2)
            return (T)__LDREXH((uint16_t *)addr);
        return (T)__LDREXW((uint32_t *)addr);
    }
}

class Atomic
{
    Atomic();

public:
    template <class T>
    static T Fetch(volatile const T *ptr)
    {
        __DSB();
        T value = *ptr;
        return value;
    }

    template <class T, class T2>
    static bool CompareExchange(T *ptr, T2 oldValue, T2 newValue)
    {
        if (Private::LDREX(ptr) == oldValue)
            return Private::STREX(newValue, ptr) == 0;
        __CLREX();
        return false;
    }
};
#undef DECLARE_OP
