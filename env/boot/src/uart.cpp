#include "uart.hpp"

#include <stm32cpp/IO.hpp>
#include <stm32cpp/UART.hpp>

#include "cli.hpp"

using namespace STM32;

#define UART1_RX_BUFFER_SIZE 255
#define UART1_TX_BUFFER_SIZE 255

static volatile uint8_t rxBuffer[UART1_RX_BUFFER_SIZE];
static volatile uint8_t txBuffer[UART1_TX_BUFFER_SIZE];

static void UART1_ErrorHandler(void);
static void UART1_RxIdleHandler(void);

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

static void UART1_RxIdleHandler()
{
    // Handle input
    Console::instance().process((char *)uartDataBuf, UART1_Driver::getRXLen());

    // Restart listen
    UART1_Driver::listen((uint8_t *)uartDataBuf, 256, UART1_RxIdleHandler);
}

extern "C" void USART1_IRQHandler(void)
{
    UART1_Driver::dispatchIRQ();
}