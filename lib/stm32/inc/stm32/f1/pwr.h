#ifndef __STM32_F1_PWR_H__
#define __STM32_F1_PWR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stm32f1xx.h>

__STATIC_FORCEINLINE
void pwr_backup_domain_wp_disable(void)
{
    PWR->CR |= PWR_CR_DBP;
}

__STATIC_FORCEINLINE
void pwr_backup_domain_wp_enable(void)
{
    PWR->CR &= ~PWR_CR_DBP;
}

#ifdef __cplusplus
}
#endif

#endif // __STM32_F1_PWR_H__