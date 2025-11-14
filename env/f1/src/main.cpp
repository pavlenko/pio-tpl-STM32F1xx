#include <stm32f1xx_hal.h>

#include <string.h>

#include <stm32/dev/clock.hpp>
#include <stm32/dev/flash.hpp>
#include <stm32/dev/io.hpp>
#include <stm32/dev/i2c.hpp>
#include <stm32/dev/uart.hpp>

#include <stm32/lib/buffer.hpp>
#include <stm32/lib/delay.hpp>

using namespace STM32;
using namespace STM32::Clock;

using LED = IO::PC13;
using UART1Tx = IO::PA9;
using UART1Rx = IO::PA10;

static uint8_t rxBuf[255];
static Buffer<1024> txBuf;

static void UART1_RxCallback(DMA::Event e);
static void UART1_TxCallback(DMA::Event e);

int main(void)
{
    // Clock config HSE 8MHz -> PLL -> SYS 72 MHz
    HSEClock::on();
    PLLClock::configure<PLLClock::Source::HSE, PLLClockConfig<9u, 1u, false>>();
    PLLClock::on();
    SysClock::configure<SysClock::Source::PLL, Flash::Latency::WS2, SysClockConfig<AHBClock::Divider::DIV1, APB1Clock::Divider::DIV2, APB2Clock::Divider::DIV1>>();
    //  Clock config end

    LED::port::enable();
    LED::configure<IO::Config<IO::Mode::OUTPUT>>();

    IO::PA::enable();
    UART1Tx::configure<IO::Config<IO::Mode::FUNCTION>>();
    UART1Rx::configure<IO::Config<IO::Mode::INPUT>>();

    UART1::configure<9600u, UART::Config::ENABLE_RX_TX>();

    Delay::init();

    UART1::rxDMA(rxBuf, 255, UART1_RxCallback);
    while (true) {
        LED::tog();
        Delay::ms(500);
    }
    return 0;
}

static void UART1_RxCallback(DMA::Event e)
{
    // Reply
    txBuf.seek(0);
    txBuf.write("RX: ");
    txBuf.write(rxBuf, 255 - UART1::DMARx::getRemaining());
    txBuf.write("\n");

    UART1::txDMA(txBuf.data(), txBuf.size(), UART1_TxCallback);
}

static void UART1_TxCallback(DMA::Event e)
{
    // Restart RX
    UART1::rxDMA(rxBuf, 255, UART1_RxCallback);
}

extern "C" void SysTick_Handler(void)
{
    Delay::dispatchIRQ();
}

extern "C" void DMA1_Channel4_IRQHandler(void)
{
    UART1::DMATx::dispatchIRQ();
}

extern "C" void DMA1_Channel5_IRQHandler(void)
{
    UART1::DMARx::dispatchIRQ();
}