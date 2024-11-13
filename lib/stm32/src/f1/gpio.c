#include <stm32/f1/gpio.h>

void gpio_set_mode(GPIO_TypeDef *port, uint8_t mode, uint8_t conf, uint16_t pins)
{
    uint8_t i, offset;
    for (i = 0; i < 16; i++) {
        if (!((1 << i) & pins)) {
            continue;
        }

        if (i < 8) {
            offset = (i * 4);
            port->CRL = (port->CRL & ~(0xF << offset)) | (mode << offset) | (conf << (offset + 2));
        } else {
            offset = ((i - 8) * 4);
            port->CRH = (port->CRH & ~(0xF << offset)) | (mode << offset) | (conf << (offset + 2));
        }
    }
}
