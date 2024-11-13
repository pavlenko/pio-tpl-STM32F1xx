#ifndef __STM32_F1_RCC_H__
#define __STM32_F1_RCC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stm32f1xx.h>

/* RCC register offsets from RCC_BASE */
#define RCC_OFFSET_CR       (0x00U)
#define RCC_OFFSET_CFGR     (0x04U)
#define RCC_OFFSET_CIR      (0x08U)
#define RCC_OFFSET_APB2RSTR (0x0CU)
#define RCC_OFFSET_APB1RSTR (0x10U)
#define RCC_OFFSET_AHBENR   (0x14U)
#define RCC_OFFSET_APB2ENR  (0x18U)
#define RCC_OFFSET_APB1ENR  (0x1CU)
#define RCC_OFFSET_BDCR     (0x20U)
#define RCC_OFFSET_CSR      (0x24U)

/* RCC compose reg+bit mask */
#define RCC_REG_BIT(base, bit) (((base) << 5) + (bit))

/* RCC decode reg from reg+bit mask */
#define RCC_REG(i) (*(__IO uint32_t *)(RCC_BASE + ((i) >> 5)))

/* RCC convert reg+bit mask to bit mask */
#define RCC_BIT(i) (1 << ((i) & 0x1F))

/* RCC oscilator defs */
enum rcc_osc
{
    RCC_PLL  = RCC_REG_BIT(RCC_OFFSET_CR, 24),
    RCC_PLL2 = RCC_REG_BIT(RCC_OFFSET_CR, 26),
    RCC_PLL3 = RCC_REG_BIT(RCC_OFFSET_CR, 28),
    RCC_HSE  = RCC_REG_BIT(RCC_OFFSET_CR, 16),
    RCC_HSI  = RCC_REG_BIT(RCC_OFFSET_CR, 0),
    RCC_LSE  = RCC_REG_BIT(RCC_OFFSET_BDCR, 0),
    RCC_LSI  = RCC_REG_BIT(RCC_OFFSET_CSR, 0),
};

/**
 * V = value line F100
 * N = standard line F101, F102, F103
 * C = communication line F105, F107
 */
enum rcc_periph_clken
{
    /* AHB peripherals */
    RCC_DMA1 = RCC_REG_BIT(RCC_OFFSET_AHBENR, 0),      /*VNC*/
    RCC_DMA2 = RCC_REG_BIT(RCC_OFFSET_AHBENR, 1),      /*VNC*/
    RCC_SRAM = RCC_REG_BIT(RCC_OFFSET_AHBENR, 2),      /*VNC*/
    RCC_FLTF = RCC_REG_BIT(RCC_OFFSET_AHBENR, 4),      /*VNC*/
    RCC_CRC = RCC_REG_BIT(RCC_OFFSET_AHBENR, 6),       /*VNC*/
    RCC_FSMC = RCC_REG_BIT(RCC_OFFSET_AHBENR, 8),      /*VN-*/
    RCC_SDIO = RCC_REG_BIT(RCC_OFFSET_AHBENR, 10),     /*-N-*/
    RCC_OTGFS = RCC_REG_BIT(RCC_OFFSET_AHBENR, 12),    /*--C*/
    RCC_ETHMAC = RCC_REG_BIT(RCC_OFFSET_AHBENR, 14),   /*--C*/
    RCC_ETHMACTX = RCC_REG_BIT(RCC_OFFSET_AHBENR, 15), /*--C*/
    RCC_ETHMACRX = RCC_REG_BIT(RCC_OFFSET_AHBENR, 16), /*--C*/

    /* APB2 peripherals */
    RCC_AFIO = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 0),    /*VNC*/
    RCC_GPIOA = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 2),   /*VNC*/
    RCC_GPIOB = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 3),   /*VNC*/
    RCC_GPIOC = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 4),   /*VNC*/
    RCC_GPIOD = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 5),   /*VNC*/
    RCC_GPIOE = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 6),   /*VNC*/
    RCC_GPIOF = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 7),   /*VN-*/
    RCC_GPIOG = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 8),   /*VN-*/
    RCC_ADC1 = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 9),    /*VNC*/
    RCC_ADC2 = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 10),   /*-NC*/
    RCC_TIM1 = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 11),   /*VNC*/
    RCC_SPI1 = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 12),   /*VNC*/
    RCC_TIM8 = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 13),   /*-N-*/
    RCC_USART1 = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 14), /*VNC*/
    RCC_ADC3 = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 15),   /*-N-*/
    RCC_TIM15 = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 16),  /*V--*/
    RCC_TIM16 = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 17),  /*V--*/
    RCC_TIM17 = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 18),  /*V--*/
    RCC_TIM9 = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 19),   /*-N-*/
    RCC_TIM10 = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 20),  /*-N-*/
    RCC_TIM11 = RCC_REG_BIT(RCC_OFFSET_APB2ENR, 21),  /*-N-*/

    /* APB1 peripherals */
    RCC_TIM2 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 0),    /*VNC*/
    RCC_TIM3 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 1),    /*VNC*/
    RCC_TIM4 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 2),    /*VNC*/
    RCC_TIM5 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 3),    /*VNC*/
    RCC_TIM6 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 4),    /*VNC*/
    RCC_TIM7 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 5),    /*VNC*/
    RCC_TIM12 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 6),   /*VN-*/
    RCC_TIM13 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 7),   /*VN-*/
    RCC_TIM14 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 8),   /*VN-*/
    RCC_WWDG = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 11),   /*VNC*/
    RCC_SPI2 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 14),   /*VNC*/
    RCC_SPI3 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 15),   /*VNC*/
    RCC_USART2 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 17), /*VNC*/
    RCC_USART3 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 18), /*VNC*/
    RCC_UART4 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 19),  /*VNC*/
    RCC_UART5 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 20),  /*VNC*/
    RCC_I2C1 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 21),   /*VNC*/
    RCC_I2C2 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 22),   /*VNC*/
    RCC_USB = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 23),    /*-N-*/
    RCC_CAN = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 25),    /*-N-*/
    RCC_CAN1 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 25),   /*--C*/
    RCC_CAN2 = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 26),   /*--C*/
    RCC_BKP = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 27),    /*VNC*/
    RCC_PWR = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 28),    /*VNC*/
    RCC_DAC = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 29),    /*VNC*/
    RCC_CEC = RCC_REG_BIT(RCC_OFFSET_APB1ENR, 30),    /*V--*/
};

#define RCC_CFGR_PLLSRC_HSI_CLK_DIV2 0x0
#define RCC_CFGR_PLLSRC_HSE_CLK      0x1
#define RCC_CFGR_PLLSRC_PREDIV1_CLK  0x1 /* On conn. line */

#define RCC_CFGR_PLLXTPRE_HSE_CLK      0x0
#define RCC_CFGR_PLLXTPRE_HSE_CLK_DIV2 0x1

#define RCC_CFGR_HPRE_SYSCLK_NODIV  0x0
#define RCC_CFGR_HPRE_SYSCLK_DIV2   0x8
#define RCC_CFGR_HPRE_SYSCLK_DIV4   0x9
#define RCC_CFGR_HPRE_SYSCLK_DIV8   0xa
#define RCC_CFGR_HPRE_SYSCLK_DIV16  0xb
#define RCC_CFGR_HPRE_SYSCLK_DIV64  0xc
#define RCC_CFGR_HPRE_SYSCLK_DIV128 0xd
#define RCC_CFGR_HPRE_SYSCLK_DIV256 0xe
#define RCC_CFGR_HPRE_SYSCLK_DIV512 0xf

#define RCC_CFGR_PPRE_SYSCLK_NODIV 0x0
#define RCC_CFGR_PPRE_SYSCLK_DIV2  0x4
#define RCC_CFGR_PPRE_SYSCLK_DIV4  0x5
#define RCC_CFGR_PPRE_SYSCLK_DIV8  0x6
#define RCC_CFGR_PPRE_SYSCLK_DIV16 0x7

#define RCC_CFGR_ADCPRE_SYSCLK_DIV2 0x0
#define RCC_CFGR_ADCPRE_SYSCLK_DIV4 0x1
#define RCC_CFGR_ADCPRE_SYSCLK_DIV6 0x2
#define RCC_CFGR_ADCPRE_SYSCLK_DIV8 0x3

#define RCC_CFGR_USBPRE_PLL_CLK_DIV1_5  0x0
#define RCC_CFGR_USBPRE_PLL_CLK_NODIV   0x1

#define RCC_CFGR_PLLMUL_PLL_CLK_MUL2		0x0 /* (XX) */
#define RCC_CFGR_PLLMUL_PLL_CLK_MUL3		0x1 /* (XX) */
#define RCC_CFGR_PLLMUL_PLL_CLK_MUL4		0x2
#define RCC_CFGR_PLLMUL_PLL_CLK_MUL5		0x3
#define RCC_CFGR_PLLMUL_PLL_CLK_MUL6		0x4
#define RCC_CFGR_PLLMUL_PLL_CLK_MUL7		0x5
#define RCC_CFGR_PLLMUL_PLL_CLK_MUL8		0x6
#define RCC_CFGR_PLLMUL_PLL_CLK_MUL9		0x7
#define RCC_CFGR_PLLMUL_PLL_CLK_MUL10		0x8 /* (XX) */
#define RCC_CFGR_PLLMUL_PLL_CLK_MUL11		0x9 /* (XX) */
#define RCC_CFGR_PLLMUL_PLL_CLK_MUL12		0xa /* (XX) */
#define RCC_CFGR_PLLMUL_PLL_CLK_MUL13		0xb /* (XX) */
#define RCC_CFGR_PLLMUL_PLL_CLK_MUL14		0xc /* (XX) */
#define RCC_CFGR_PLLMUL_PLL_CLK_MUL15		0xd /* 0xd: PLL x 15 */
#define RCC_CFGR_PLLMUL_PLL_CLK_MUL6_5		0xd /* 0xd: PLL x 6.5 for conn. line */
#define RCC_CFGR_PLLMUL_PLL_CLK_MUL16		0xe /* (XX) */

/* Union of all options for f100 ... f107 */
struct rcc_clock_preset {
    uint8_t pll_mul;
    uint8_t pll_source;
    uint8_t xtpre;
    uint8_t hpre;
    uint8_t ppre1;
    uint8_t ppre2;
    uint8_t adcpre;
    uint8_t flash_ws;
    uint8_t prediv1; /* aka xtpre, only one bit on smaller parts */
    uint8_t prediv1_source;
    uint8_t prediv2;
    uint8_t pll2_mul;
    uint8_t pll3_mul;
    uint8_t usbpre;
};

enum rcc_clock_preset_hse {
    RCC_CLOCK_HSE_8MHZ_TO_24MHZ,
    RCC_CLOCK_HSE_8MHZ_TO_48MHZ,
    RCC_CLOCK_HSE_8MHZ_TO_72MHZ,
    RCC_CLOCK_HSE_END
};

extern const struct rcc_clock_preset rcc_clock_hse_presets[RCC_CLOCK_HSE_END];

void rcc_clock_setup_pll(const struct rcc_clock_preset *preset);

__STATIC_FORCEINLINE
void rcc_osc_enable(enum rcc_osc osc)
{
    RCC_REG(osc) |= RCC_BIT(osc);
}

__STATIC_FORCEINLINE
void rcc_osc_disable(enum rcc_osc osc)
{
    RCC_REG(osc) &= ~RCC_BIT(osc);
}

__STATIC_FORCEINLINE
bool rcc_osc_is_ready(enum rcc_osc osc)
{
    return RCC_REG(osc) & (RCC_BIT(osc) << 1);
}

__STATIC_FORCEINLINE
void rcc_periph_clk_enable(enum rcc_periph_clken clken)
{
    RCC_REG(clken) |= RCC_BIT(clken);
}

__STATIC_FORCEINLINE
void rcc_periph_clk_disable(enum rcc_periph_clken clken)
{
    RCC_REG(clken) &= ~RCC_BIT(clken);
}

#ifdef __cplusplus
}
#endif

#endif // __STM32_F1_RCC_H__