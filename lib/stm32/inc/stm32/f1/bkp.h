#ifndef __STM32_F1_BKP_H__
#define __STM32_F1_BKP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stm32f1xx.h>

#define BKP_DR(reg) (*(__IO uint32_t *)(BKP_BASE + 4 + (4 * (reg))))

enum bkp_reg {
    BKP1,
    BKP2,
    BKP3,
    BKP4,
    BKP5,
    BKP6,
    BKP7,
    BKP8,
    BKP9,
    BKP10,
};

__STATIC_FORCEINLINE
uint16_t bkp_get(enum bkp_reg reg)
{
    return BKP_DR(reg);
}

void bkp_set(enum bkp_reg reg, uint16_t val);

#ifdef __cplusplus
}
#endif

#endif // __STM32_F1_BKP_H__