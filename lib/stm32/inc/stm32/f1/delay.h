#ifndef __STM32_F1_DELAY_H__
#define __STM32_F1_DELAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stm32f1xx.h>

void delay_setup(void);

void delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif // __STM32_F1_DELAY_H__