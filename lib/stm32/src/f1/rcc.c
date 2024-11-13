#include <stm32/f1/flash.h>
#include <stm32/f1/rcc.h>

const struct rcc_clock_preset rcc_clock_hse_presets[RCC_CLOCK_HSE_END] = {
    {
        /* hse8, pll to 48 */
        .pll_mul = RCC_CFGR_PLLMUL_PLL_CLK_MUL6,
        .pll_source = RCC_CFGR_PLLSRC_HSE_CLK,
        .hpre = RCC_CFGR_HPRE_SYSCLK_NODIV,
        .ppre1 = RCC_CFGR_PPRE_SYSCLK_DIV2,
        .ppre2 = RCC_CFGR_PPRE_SYSCLK_NODIV,
        .adcpre = RCC_CFGR_ADCPRE_SYSCLK_DIV4,
        .flash_ws = FLASH_LATENCY_1WS,
    },
    {
        /* hse8, pll to 72 */
        .pll_mul = RCC_CFGR_PLLMUL_PLL_CLK_MUL9,
        .pll_source = RCC_CFGR_PLLSRC_HSE_CLK,
        .hpre = RCC_CFGR_HPRE_SYSCLK_NODIV,
        .ppre1 = RCC_CFGR_PPRE_SYSCLK_DIV2,
        .ppre2 = RCC_CFGR_PPRE_SYSCLK_NODIV,
        .adcpre = RCC_CFGR_ADCPRE_SYSCLK_DIV6,
        .flash_ws = FLASH_LATENCY_2WS,
    },
};

void rcc_clock_setup_pll(const struct rcc_clock_preset *preset)
{
    if (preset->pll_source == RCC_CFGR_PLLSRC_HSE_CLK) {
        rcc_osc_enable(RCC_HSE);
        while(!rcc_osc_is_ready(RCC_HSE));
    } else {
        rcc_osc_enable(RCC_HSI);
        while(!rcc_osc_is_ready(RCC_HSI));
    }

    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE) | (preset->hpre << RCC_CFGR_HPRE_Pos);
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE1) | (preset->ppre1 << RCC_CFGR_PPRE1_Pos);
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE2) | (preset->ppre2 << RCC_CFGR_PPRE2_Pos);
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_ADCPRE) | (preset->adcpre << RCC_CFGR_ADCPRE_Pos);

    if (preset->usbpre) {
        RCC->CFGR |= RCC_CFGR_USBPRE;
    } else {
        RCC->CFGR &= ~RCC_CFGR_USBPRE;
    }

    flash_set_ws(preset->flash_ws);

    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PLLMULL) | (preset->pll_mul << RCC_CFGR_PLLMULL_Pos);
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PLLSRC) | (preset->pll_source << RCC_CFGR_PLLSRC_Pos);
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PLLXTPRE) | (preset->xtpre << RCC_CFGR_PLLXTPRE_Pos);

 #ifdef RCC_CFGR2_PREDIV1
    if (preset->prediv1) {
       RCC->CFGR2 = (RCC->CFGR2 & ~RCC_CFGR2_PREDIV1) | (preset->prediv1 << RCC_CFGR2_PREDIV1_Pos);
    }

    if (preset->prediv1_source) {
        RCC->CFGR2 |= RCC_CFGR2_PREDIV1SRC;
    } else {
        RCC->CFGR2 &= ~RCC_CFGR2_PREDIV1SRC;
    }
#endif

#ifdef RCC_CFGR2_PREDIV2
    if (preset->prediv2) {
        RCC->CFGR2 = (RCC->CFGR2 & ~RCC_CFGR2_PREDIV2) | (preset->prediv2 << RCC_CFGR2_PREDIV2_Pos);
    }
#endif

#ifdef RCC_CFGR2_PLL2MUL
    if (preset->pll2_mul) {
        RCC->CFGR2 = (RCC->CFGR2 & ~RCC_CFGR2_PLL2MUL) | (preset->pll2_mul << RCC_CFGR2_PLL2MUL_Pos);
        rcc_osc_enable(RCC_PLL2);
        while(!rcc_osc_is_ready(RCC_PLL2));
    }
#endif

#ifdef RCC_CFGR2_PLL3MUL
    if (preset->pll3_mul) {
        RCC->CFGR2 = (RCC->CFGR2 & ~RCC_CFGR2_PLL3MUL) | (preset->pll2_mul << RCC_CFGR2_PLL3MUL_Pos);
        rcc_osc_enable(RCC_PLL3);
        while(!rcc_osc_is_ready(RCC_PLL3));
    }
#endif

    /* Enable PLL oscillator and wait for it to stabilize. */
    rcc_osc_enable(RCC_PLL);
    while(!rcc_osc_is_ready(RCC_PLL));

    /* Select PLL as SYSCLK source. */
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
}
