#include <stm32f0xx_hal.h>

#include <string.h>

#include <stm32/dev/clock.hpp>
#include <stm32/dev/flash.hpp>
#include <stm32/dev/io.hpp>
#include <stm32/dev/i2c.hpp>
#include <stm32/dev/uart.hpp>

#include <stm32/drv/ssd1306.hpp>

#include <stm32/lib/buffer.hpp>
#include <stm32/lib/delay.hpp>

using namespace STM32;
using namespace STM32::Clock;

using LED = IO::PA4;
using UART1Tx = IO::PA9;
using UART1Rx = IO::PA10;

static uint8_t rxBuf[255];
static Buffer<1024> txBuf;

static void UART1_RxCallback(DMA::Event e, uint16_t n);
static void UART1_TxCallback(DMA::Event e, uint16_t n);

int main(void)
{
    // Clock config HSE 8MHz -> PLL -> SYS 48 MHz
    HSEClock::on();
    PLLClock::configure<PLLClock::Source::HSE_PREDIV, PLLClockConfig<5u, 0u, false>>();
    PLLClock::on();
    SysClock::configure<SysClock::Source::PLL, Flash::Latency::WS1, SysClockConfig<AHBClock::Divider::DIV1, APBClock::Divider::DIV1>>();
    //  Clock config end

    Delay::init();

    LED::port::enable();
    LED::configure<IO::Config<IO::Mode::OUTPUT>>();

    IO::PA::enable();
    UART1Tx::configure<IO::Config<IO::Mode::FUNCTION>>();
    UART1Tx::setAltFunction<IO::AF::AF1>();
    UART1Rx::configure<IO::Config<IO::Mode::FUNCTION>>();
    UART1Rx::setAltFunction<IO::AF::AF1>();

    UART1::configure<9600u, UART::Config::ENABLE_RX_TX>();
    UART1::rxDMA(rxBuf, 255, UART1_RxCallback);

    I2C1::Master::select(0x78 >> 1, I2C::Speed::STANDARD);
    SSD1306<I2C1>::init();

    while (true) {
        LED::tog();
        Delay::ms(500);
    }
    return 0;
}

static void UART1_RxCallback(DMA::Event e, uint16_t n)
{
    // Reply
    txBuf.seek(0);
    txBuf.write("RX: ");
    txBuf.write(rxBuf, 255 - UART1::DMARx::getRemaining());
    txBuf.write("\n");

    UART1::txDMA(txBuf.data(), txBuf.size(), UART1_TxCallback);
}

static void UART1_TxCallback(DMA::Event e, uint16_t n)
{
    // Restart RX
    UART1::rxDMA(rxBuf, 255, UART1_RxCallback);
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

extern "C" void I2C1_IRQHandler(void)
{
    I2C1::Slave::dispatchErrorIRQ();
}
