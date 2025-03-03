#include <stm32f1xx.h>

int main(void) {
    // rcc_clock_setup_pll(&rcc_clock_hse_presets[RCC_CLOCK_HSE_8MHZ_TO_72MHZ]);

    // delay_setup();

    // rcc_periph_clk_enable(RCC_GPIOC);

    // gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CONF_OUTPUT_PP, GPIO13);

    //TODO use cdcacm example & create composite device for user env

    while (1) {
        // gpio_set(GPIOC, GPIO13);
        // delay_ms(1000);
        // gpio_clr(GPIOC, GPIO13);
        // delay_ms(1000);
    }
}