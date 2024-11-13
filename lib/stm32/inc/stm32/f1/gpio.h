#ifndef __STM32_F1_GPIO_H__
#define __STM32_F1_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stm32f1xx.h>

#define GPIO0    (1 << 0)
#define GPIO1    (1 << 1)
#define GPIO2    (1 << 2)
#define GPIO3    (1 << 3)
#define GPIO4    (1 << 4)
#define GPIO5    (1 << 5)
#define GPIO6    (1 << 6)
#define GPIO7    (1 << 7)
#define GPIO8    (1 << 8)
#define GPIO9    (1 << 9)
#define GPIO10   (1 << 10)
#define GPIO11   (1 << 11)
#define GPIO12   (1 << 12)
#define GPIO13   (1 << 13)
#define GPIO14   (1 << 14)
#define GPIO15   (1 << 15)
#define GPIO_ALL 0xFFFF

#define GPIO_MODE_INPUT         0x0 /* Default */
#define GPIO_MODE_OUTPUT_10_MHZ 0x1
#define GPIO_MODE_OUTPUT_2_MHZ  0x2
#define GPIO_MODE_OUTPUT_50_MHZ 0x3

#define GPIO_CONF_INPUT_ANALOG      0x0
#define GPIO_CONF_INPUT_FLOAT       0x1 /* Default */
#define GPIO_CONF_INPUT_PULL_UPDOWN 0x2
#define GPIO_CONF_INPUT_RESERVED    0x3

#define GPIO_CONF_OUTPUT_PP    0x0
#define GPIO_CONF_OUTPUT_OD    0x1
#define GPIO_CONF_OUTPUT_AF_PP 0x2
#define GPIO_CONF_OUTPUT_AF_OD 0x3

__STATIC_FORCEINLINE
uint16_t gpio_get(GPIO_TypeDef *port, uint16_t pins)
{
    return (uint16_t) (port->IDR & pins);
}

__STATIC_FORCEINLINE
void gpio_set(GPIO_TypeDef *port, uint16_t pins)
{
    port->BSRR = pins;
}

__STATIC_FORCEINLINE
void gpio_clr(GPIO_TypeDef *port, uint16_t pins)
{
    port->BSRR = (pins << 16);
}

__STATIC_FORCEINLINE
void gpio_toggle(GPIO_TypeDef *port, uint16_t pins)
{
    port->BSRR = ((port->ODR & pins) << 16) | (~port->ODR & pins);
}

void gpio_set_mode(GPIO_TypeDef *port, uint8_t mode, uint8_t conf, uint16_t pins);

#ifdef __cplusplus
}
#endif

#endif // __STM32_F1_GPIO_H__