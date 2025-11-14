#define HSE_VALUE 8000000U

#include <stm32f4xx_hal.h>

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

using LED = IO::PB2;
using UART1Tx = IO::PA9;
using UART1Rx = IO::PA10;

static uint8_t rxBuf[255];
static Buffer<1024> txBuf;

static void UART1_RxCallback(DMA::Event e);
static void UART1_TxCallback(DMA::Event e);

int main(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    // Clock config HSE 8MHz -> PLL -> SYS 168 MHz
    HSEClock::on();
    PLLClock::configure<PLLClock::Source::HSE, PLLClockConfig<8u, 336u, 2u, 7u, 2u>>();
    PLLClock::on();
    SysClock::configure<SysClock::Source::PLL, Flash::Latency::WS5, SysClockConfig<AHBClock::Divider::DIV1, APB1Clock::Divider::DIV4, APB2Clock::Divider::DIV2>>();
    // Clock config end

    LED::port::enable();
    LED::configure<IO::Config<IO::Mode::OUTPUT>>();

    IO::PA::enable();
    UART1Tx::configure<IO::Config<IO::Mode::FUNCTION>>();
    UART1Tx::setAltFunction<IO::AF::AF1>();
    UART1Rx::configure<IO::Config<IO::Mode::FUNCTION>>();
    UART1Rx::setAltFunction<IO::AF::AF1>();

    UART1::configure<9600u, UART::Config::ENABLE_RX_TX>();

    Delay::init();

    //I2C1::Master::select(SSD1306<I2C1>::address, I2C::Speed::FAST);
    //SSD1306<I2C1>::init();

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

extern "C" void DMA2_Stream5_IRQHandler(void)
{
    UART1::DMARx::dispatchIRQ();
}

extern "C" void DMA2_Stream7_IRQHandler(void)
{
    UART1::DMATx::dispatchIRQ();
}

// extern "C" void I2C1_EV_IRQHandler(void)
// {
//     I2C1::Slave::dispatchEventIRQ();
// }

// extern "C" void I2C1_ER_IRQHandler(void)
// {
//     I2C1::Slave::dispatchErrorIRQ();
// }
