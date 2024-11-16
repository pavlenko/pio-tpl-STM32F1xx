#include <stm32f1xx_hal.h>

// Suppress compiler warnings, must be before main includes
__attribute__((weak)) void _close_r(void) {}
__attribute__((weak)) void _lseek(void) {}
__attribute__((weak)) void _read_r(void) {}
__attribute__((weak)) void _write_r(void) {}

#include <string.h>

// #include <console.hpp>
// #include <console_v2.hpp>
#include <stm32cpp/Clock.hpp>
#include <stm32cpp/Delay.hpp>
#include <stm32cpp/Dispatcher.hpp>
#include <stm32cpp/I2C.hpp>
#include <stm32cpp/SPI.hpp>
#include <stm32cpp/IO.hpp>
// #include <stm32cpp/UART.hpp>

#include "uart.hpp"
#include "cli.hpp"

// https://github.com/Nanaud7/shell-stm32/blob/main/stm32-bare-metal/shell.c

#define TEST_CLOCK 1
#define TEST_FLASH 1
#define TEST_IO 1
#define TEST_I2C 0
#define TEST_SPI 0
#define TEST_UART 1

volatile uint32_t delay = 500;

// static inline void serial_write(const char *str)
// {
//     STM32::UART1_Driver::send((uint8_t *)str, strlen(str), nullptr);
// }

int main(void)
{
    using namespace STM32;

    // Clock config
    Clock::HSEClock::on();

    Clock::PLLClock::configure<Clock::PLLClockConfig<Clock::PLLClockSource::HSE, RCC_CFGR_PLLMULL9, 0u>>();
    Clock::PLLClock::on();

    using SYSClockConfig = Clock::SYSClockConfig<
        Clock::SYSClockSource::PLL,
        RCC_CFGR_HPRE_DIV1,
        RCC_CFGR_PPRE1_DIV2,
        RCC_CFGR_PPRE2_DIV1,
        FLASH_LATENCY_2>;
    Clock::SYSClock::configure<SYSClockConfig>();

    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    // Clock config end

    Delay::init();

    // Led config
    IO::PC::enable();
    IO::PC13::configure<IO::Mode::OUTPUT>(IO::Speed::LOW);
    // Led config end

    UART1_Init();
    CLI_Init();

    while (true)
    {
        // Dispatcher::dispatch();

        // Console::process(nullptr, 0);

        IO::PC13::tog();
        Delay::ms(delay);
        CLI::write("OK\n");
        Delay::ms(delay);
    }
    return 0;
}

extern "C"
{
    void SysTick_Handler(void)
    {
        STM32::Delay::dispatchIRQ();
    }

#if TEST_I2C == 1
    void I2C1_EV_IRQHandler(void)
    {
        STM32::I2C1_Driver::instance().dispatchEventIRQ();
    }

    void I2C1_ER_IRQHandler(void)
    {
        STM32::I2C1_Driver::instance().dispatchErrorIRQ();
    }
#endif
}
