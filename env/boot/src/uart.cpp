#include "uart.hpp"

#include <Dispatcher.hpp>
#include <stm32cpp/IO.hpp>
#include <stm32cpp/UART.hpp>

#include "cli.hpp"

namespace UART1
{
#if defined(UART1_RX_BUFFER_SIZE)
    static const size_t RX_BUFFER_SIZE = UART1_RX_BUFFER_SIZE;
#else
    static const size_t RX_BUFFER_SIZE = 64;
#endif
#if defined(UART1_TX_BUFFER_SIZE)
    static const size_t TX_BUFFER_SIZE = UART1_TX_BUFFER_SIZE;
#else
    static const size_t TX_BUFFER_SIZE = 64;
#endif

    static volatile uint8_t rxBuffer[RX_BUFFER_SIZE];
    static volatile uint8_t txBuffer[TX_BUFFER_SIZE];

    static void RxIdleIRQHandler();
    static void RxIdleEvent();

    void init()
    {
        using namespace STM32;

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

        UART1_Driver::listen((uint8_t *)rxBuffer, RX_BUFFER_SIZE, RxIdleIRQHandler);
    }

    //tmp
    void write(const char *str)
    {
        write((uint8_t *)str, strlen(str));
    }

    //tmp
    void write(uint8_t *buf, size_t len)
    {
        STM32::UART1_Driver::send(buf, len, nullptr);
    }

    static void RxIdleIRQHandler()
    {
        // STM32::IO::PC::disable();
        // Dispatcher::pushTask(RxIdleEvent);
        Console::instance().process((char *)rxBuffer, STM32::UART1_Driver::getRXLen());

        STM32::UART1_Driver::listen((uint8_t *)rxBuffer, RX_BUFFER_SIZE, RxIdleIRQHandler);
    }

    static void RxIdleEvent()
    {
        STM32::IO::PC::disable();
        // Console::instance().process((char *)rxBuffer, STM32::UART1_Driver::getRXLen());

        // STM32::UART1_Driver::listen((uint8_t *)rxBuffer, RX_BUFFER_SIZE, RxIdleIRQHandler);
    }
}

extern "C" void USART1_IRQHandler(void)
{
    STM32::UART1_Driver::dispatchIRQ();
}