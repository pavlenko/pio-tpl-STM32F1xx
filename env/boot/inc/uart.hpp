#ifndef __UART_HPP__
#define __UART_HPP__

#include <stdint.h>
#include <stddef.h>

namespace UART1
{
    void init();
    void write(const char *str);
    void write(uint8_t *buf, size_t len);
}

#endif // __UART_HPP__