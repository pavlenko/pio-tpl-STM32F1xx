#if defined(STM32F0)
#include <stm32cpp/F0/Flash.hpp>
#elif defined(STM32F1)
#include <stm32cpp/F1/Flash.hpp>
#elif defined(STM32F4)
#include <stm32cpp/F4/Flash.hpp>
#else
#error "STM32YYNN chip series is not defined"
#endif