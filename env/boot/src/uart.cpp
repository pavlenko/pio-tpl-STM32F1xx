#include "uart.hpp"

#include <stm32cpp/IO.hpp>
#include <stm32cpp/UART.hpp>

#include "console.hpp"

using namespace STM32;

static void UART1_ErrorHandler(void);
static void UART1_RxIdleHandler(size_t len);
static void UART1_TxDoneHandler(void);

volatile uint8_t uartDataBuf[256];

void UART1_Init()
{
    // UART1 GPIO (9-TX, 10-RX)
    IO::PA::enable();
    IO::PA9::configure<IO::Mode::ALTERNATE>(IO::Speed::FAST);
    IO::PA10::configure<IO::Mode::INPUT>(IO::Pull::PULL_UP); //<-- require pull-up if host has not it

    UART1_Driver::init<UART::Config<
        UART::Mode::RX_TX,
        9600,
        UART::DataBits::_8BIT,
        UART::StopBits::_2BIT,
        UART::Parity::NONE>>();
    UART1_Driver::setErrorHandler(UART1_ErrorHandler);
    UART1_Driver::listen((uint8_t *)uartDataBuf, 256, UART1_RxIdleHandler);
}

static void UART1_ErrorHandler(void)
{
    // Do nothing for now
}

static void UART1_RxIdleHandler(size_t len)
{
    // TODO handle commands here or just signal abou that
    //  also restart listen (maybe need somehow disable this requitrement)
    UART1_Driver::send((uint8_t *)uartDataBuf, len, UART1_TxDoneHandler);   // echo, but need hadle
    UART1_Driver::listen((uint8_t *)uartDataBuf, 256, UART1_RxIdleHandler); // need restart

    Console::Application::process((char *)uartDataBuf, len);
}

static void UART1_TxDoneHandler(void)
{
    // Do nothing for now
}

extern "C" void USART1_IRQHandler(void)
{
    UART1_Driver::dispatchIRQ();
}