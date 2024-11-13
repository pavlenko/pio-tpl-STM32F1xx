#ifndef __STM32_SPI_HPP__
#define __STM32_SPI_HPP__

#include <functional>
#include <stdint.h>

#include <stm32f1xx.h>

#include <_common.hpp>

enum class SPI_State: uint8_t {
    RESET      = 0x00U,
    READY      = 0x01U,
    BUSY       = 0x02U,
    BUSY_TX    = 0x03U,
    BUSY_RX    = 0x04U,
    BUSY_TX_RX = 0x05U,
    ERROR      = 0x06U,
    ABORT      = 0x07U,
};

enum class SPI_Mode: uint16_t {
    SLAVE  = 0x0000U,
    MASTER = (SPI_CR1_MSTR | SPI_CR1_SSI),
};

enum class SPI_BusLine: uint16_t {
    TWO_LINE        = 0x0000U,
    TWO_LINE_RXONLY = SPI_CR1_RXONLY,
    ONE_LINE_BIDIR  = SPI_CR1_BIDIMODE,
};

enum class SPI_BusMode: uint16_t {
    MODE_0, // CPOL = 0, CPHA = 0
    MODE_1, // CPOL = 0, CPHA = 1
    MODE_2, // CPOL = 1, CPHA = 0
    MODE_3, // CPOL = 1, CPHA = 1
};

enum class SPI_BitOrder: uint16_t {
    MSB = 0x0000U,
    LSB = SPI_CR1_LSBFIRST,
};

class SPI_Config
{
    friend class SPI_Driver;
private:
    uint32_t _clock;
    SPI_BitOrder _bitOrder;
    SPI_BusMode _busMode;
    SPI_BusLine _busLine;

public:
    constexpr SPI_Config()
        : _clock(1000000),
          _bitOrder(SPI_BitOrder::MSB),
          _busMode(SPI_BusMode::MODE_0),
          _busLine(SPI_BusLine::TWO_LINE) {};

    constexpr SPI_Config(uint32_t clk, SPI_BitOrder bo, SPI_BusMode bm, SPI_BusLine bl)
        : _clock(clk),
          _bitOrder(bo),
          _busMode(bm),
          _busLine(bl) {}
};

void SPI_onTxByte(SPI_Driver &driver);
void SPI_onTxCompleted(SPI_Driver &driver);
void SPI_onRxByte(SPI_Driver &driver);
void SPI_onRxCompleted(SPI_Driver &driver);
void SPI_onTxRxCompleted(SPI_Driver &driver);
void SPI_onAbort(SPI_Driver &driver);
void SPI_onError(SPI_Driver &driver);

class SPI_Driver
{
public:
    typedef std::function<void(void)> cbTxCompleted_t;
    typedef std::function<void(void)> cbRxCompleted_t;
    typedef std::function<void(uint8_t errors)> cbError_t;
private:
    SPI_TypeDef *_regs;
    SPI_Mode _mode;
    SPI_Config _config;
    __IO SPI_State _state;
    uint8_t *_txDataBuf;
    uint16_t _txDataLen;
    __IO uint16_t _txDataCnt;
    uint8_t *_rxDataBuf;
    uint16_t _rxDataLen;
    __IO uint16_t _rxDataCnt;
    void (*_txISR)(void);
    void (*_rxISR)(void);
    __IO uint8_t _errors;
    cbTxCompleted_t _onTxCompleted;
    cbTxCompleted_t _onRxCompleted;
    cbError_t _onError;
public:
    SPI_Driver() = delete;
    SPI_Driver(SPI_TypeDef *regs): _regs(regs) {}

    Status configure(SPI_Mode mode, SPI_Config config);
    Status send(uint8_t *data, uint16_t size);
    Status send_IRQ(uint8_t *data, uint16_t size);
    Status recv(uint8_t *data, uint16_t size);
    Status recv_IRQ(uint8_t *data, uint16_t size);
    Status transfer(uint8_t *txData, uint8_t *rxData, uint16_t size);
    Status transfer_IRQ(uint8_t *txData, uint8_t *rxData, uint16_t size);

    void dispatchIRQ(void);
};

#endif // __STM32_SPI_HPP__