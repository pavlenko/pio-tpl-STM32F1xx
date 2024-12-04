#ifndef __STM32_UART_DEFINITIONS_H__
#define __STM32_UART_DEFINITIONS_H__

#include <cstdint>

#include <stm32cpp/_common.hpp>

namespace STM32::UART
{
    enum class State : uint32_t
    {
        RESET,
        READY,
        BUSY,
    };

    enum class Event : uint32_t
    {
        NONE = 0x00000000u,
        TX_DONE = 0x00000001u,
        RX_DONE = 0x00000002u,
        RX_IDLE = 0x00000004u,
        ERROR = 0x00000008u,
    };

    inline constexpr Event operator|(Event l, Event r)
    {
        return Event(static_cast<uint32_t>(l) | static_cast<uint32_t>(r));
    }

    inline constexpr Event operator&(Event l, Event r)
    {
        return Event(static_cast<uint32_t>(l) & static_cast<uint32_t>(r));
    }

    inline constexpr Event &operator|=(Event &l, Event r)
    {
        return l = l | r;
    }

    enum class Error : uint32_t
    {
        NONE = 0x00000000U,
        PARITY = 0x00000001U,
        NOISE = 0x00000002U,
        FRAME = 0x00000004U,
        OVERRUN = 0x00000008U,
        DMA = 0x00000010U,
    };

    inline constexpr Error operator|(Error l, Error r)
    {
        return Error(static_cast<uint32_t>(l) | static_cast<uint32_t>(r));
    }

    inline constexpr Error operator&(Error l, Error r)
    {
        return Error(static_cast<uint32_t>(l) & static_cast<uint32_t>(r));
    }

    inline constexpr Error &operator|=(Error &l, Error r)
    {
        return l = l | r;
    }

    enum class Mode
    {
        NONE = 0x00000000u,
        RX = USART_CR1_RE,
        TX = USART_CR1_TE,
        RX_TX = USART_CR1_RE | USART_CR1_TE,
    };

    enum class Baud
    {
        _4800,
        _9600,
        _19200,
        _38400,
        _57600,
        _115200,
        _230400,
        _460800,
        _921600,
    };

    enum class StopBits
    {
        _1BIT = 0x00000000u,
        _2BIT = USART_CR2_STOP_1,
    };

    enum class DataBits
    {
        _8BIT = 0x00000000u,
        _9BIT = USART_CR1_M,
    };

    enum class Parity
    {
        NONE = 0x00000000u,
        EVEN = USART_CR1_PCE,
        ODD = USART_CR1_PCE | USART_CR1_PS,
    };

    enum class HWControl
    {
        NONE = 0x00000000u,
        RTS = USART_CR3_RTSE,
        CTS = USART_CR3_CTSE,
        RTS_CTS = USART_CR3_RTSE | USART_CR3_CTSE,
    };

    enum class Oversampling
    {
        _16BIT = 0x00000000u,
#if defined(USART_CR1_OVER8)
        _8BIT = USART_CR1_OVER8,
#endif
    };

    template <
        Mode tMode,
        uint32_t tBaudRate,
        DataBits tDataBits,
        StopBits tStopBits,
        Parity tParity,
        HWControl tHWControl = HWControl::NONE,
        Oversampling tOversampling = Oversampling::_16BIT>
    struct Config
    {
        static constexpr auto mode = tMode;
        static constexpr auto baudRate = tBaudRate;
        static constexpr auto dataBits = tDataBits;
        static constexpr auto stopBits = tStopBits;
        static constexpr auto parity = tParity;
        static constexpr auto hwControl = tHWControl;
        static constexpr auto oversampling = tOversampling;
    };

    //TODO template or not template???
    struct Config2
    {
        Mode mode;
        DataBits dataBits;
        StopBits stopBits;
        Parity parity;
        HWControl hwControl;
        Oversampling oversampling;
        Config2(Mode mode, DataBits dataBits, StopBits stopBits, Parity parity, HWControl hwControl, Oversampling oversampling)
        {
        }
    };

    struct Data
    {
        uint8_t *buf;
        uint16_t len;
        uint16_t cnt;
        TransferCallback cb;
    };
}

#endif // __STM32_UART_DEFINITIONS_H__