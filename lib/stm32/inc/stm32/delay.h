#ifndef __STM32_DELAY_H__
#define __STM32_DELAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STM32F1)
#include <stm32/f1/delay.h>
#elif defined(STM32F4)
#include <stm32/f4/delay.h>
#else
#error "stm32 family not defined."
#endif

#ifdef __cplusplus
}
#endif

#endif // __STM32_DELAY_H__