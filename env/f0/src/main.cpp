#include <stm32f0xx_hal.h>

#include <string.h>

#include <stm32/dev/clock.hpp>
#include <stm32/dev/flash.hpp>
#include <stm32/dev/io.hpp>
// #include <stm32/dev/i2c.hpp>
#include <stm32/dev/uart.hpp>

#include <stm32/lib/delay.hpp>

using namespace STM32;
using namespace STM32::Clock;

using LED = IO::PA4;
using UART1Tx = IO::PA9;
using UART1Rx = IO::PA10;

static uint8_t rxBuf[255];

int main(void)
{
    // Clock config HSE 8MHz -> PLL -> SYS 48 MHz
    HSEClock::on();
    PLLClock::configure<PLLClock::Source::HSE_PREDIV, PLLClockConfig<5u, 0u, false>>();
    PLLClock::on();
    SysClock::configure<SysClock::Source::PLL, Flash::Latency::WS1, SysClockConfig<AHBClock::Divider::DIV1, APBClock::Divider::DIV1>>();
    //  Clock config end

    LED::port::enable();
    LED::configure<IO::Config<IO::Mode::OUTPUT>>();

    IO::PA::enable();
    UART1Tx::configure<IO::Config<IO::Mode::FUNCTION>>();
    UART1Tx::setAltFunction<IO::AF::AF1>();
    UART1Rx::configure<IO::Config<IO::Mode::FUNCTION>>();
    UART1Rx::setAltFunction<IO::AF::AF1>();

    UART1::configure<9600u, UART::Config::ENABLE_RX_TX>();

    Delay::init();

    UART1::rxDMA(rxBuf, 255, [](DMA::Event e) {});
    while (true) {
        LED::tog();
        Delay::ms(500);
    }
    return 0;
}

extern "C" void SysTick_Handler(void)
{
    Delay::dispatchIRQ();
}

extern "C" void DMA1_Channel2_3_IRQHandler(void)
{
    UART1::DMATx::dispatchIRQ();
    UART1::DMARx::dispatchIRQ();
}
