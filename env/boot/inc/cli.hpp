#ifndef __CLI_HPP__
#define __CLI_HPP__

#include <Console.hpp>

void CLI_Writer(const char *str);

using CLI = V2::Console<CLI_Writer>;

void CLI_Init();

#endif // __CLI_HPP__