#define HSE_VALUE 8000000U

#include <stm32f4xx_hal.h>

#include <string.h>

#include <stm32/dev/clock.hpp>
#include <stm32/dev/flash.hpp>
#include <stm32/dev/io.hpp>
#include <stm32/dev/i2c.hpp>
#include <stm32/dev/uart.hpp>

#include <stm32/drv/ssd1306.hpp>
#include <stm32/lib/delay.hpp>

using namespace STM32;
using LED = IO::PB2;

int main(void)
{

    // Clock config
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    Clock::HSEClock::on();
    Clock::PLLClock::configure<
        Clock::PLLClock::Source::HSE,
        Clock::PLLClockConfig<8u, 336u, 2u, 7u, 2u> //<-- HSE 8MHz -> SYS 168 MHz
    >();
    Clock::PLLClock::on();

    Clock::SysClock::configure<
        Clock::SysClock::Source::PLL,
        Flash::Latency::WS5,
        Clock::SysClockConfig<Clock::AHBClock::Divider::DIV1, Clock::APB1Clock::Divider::DIV4, Clock::APB2Clock::Divider::DIV2>
    >();
    // Clock config end

    LED::port::enable();
    LED::configure<IO::Config<IO::Mode::OUTPUT>>();

    Delay::init();

    I2C1::Master::select(SSD1306<I2C1>::address, I2C::Speed::FAST);
    SSD1306<I2C1>::init();

    while (true)
    {
        LED::tog();
        Delay::ms(500);
    }
    return 0;
}

extern "C" void SysTick_Handler(void)
{
    Delay::dispatchIRQ();
}

extern "C" void I2C1_EV_IRQHandler(void)
{
    STM32::I2C1::Slave::dispatchEventIRQ();
}

extern "C" void I2C1_ER_IRQHandler(void)
{
    STM32::I2C1::Slave::dispatchErrorIRQ();
}
