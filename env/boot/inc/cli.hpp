#ifndef __CLI_HPP__
#define __CLI_HPP__

#include <Console.hpp>

namespace Console
{
    void Writer(uint8_t *buf, size_t len);

    App<Writer> &instance();
}

#endif // __CLI_HPP__