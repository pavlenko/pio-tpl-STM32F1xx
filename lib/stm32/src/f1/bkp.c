#include <stm32/f1/bkp.h>
#include <stm32/f1/rcc.h>
#include <stm32/f1/pwr.h>

void bkp_set(enum bkp_reg reg, uint16_t val)
{
    rcc_periph_clk_enable(RCC_PWR);
    rcc_periph_clk_enable(RCC_BKP);

    pwr_backup_domain_wp_disable();

    BKP_DR(reg) = val;

    pwr_backup_domain_wp_enable();
}
