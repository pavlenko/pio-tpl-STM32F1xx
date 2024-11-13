#if defined(STM32F0)
#include <stm32cpp/F0/Clock.hpp>
#elif defined(STM32F1)
#include <stm32cpp/F1/Clock.hpp>
#elif defined(STM32F4)
#include <stm32cpp/F4/Clock.hpp>
#else
#error "STM32YYNN chip series is not defined"
#endif
