#include <SPI.hpp>

static inline void _SPI_CLEAR_OVR(SPI_TypeDef *regs)
{
    do {
        __IO uint32_t tmp = 0x00U;
        tmp = regs->DR;
        tmp = regs->SR;
        (void) tmp;
    } while (0U);
}

static inline void _SPI_CLEAR_MODF(SPI_TypeDef *regs)
{
    do {
        __IO uint32_t tmp = 0x00U;
        tmp = regs->SR;
        CLEAR_BIT(regs->CR1, SPI_CR1_SPE);
        (void) tmp;
    } while (0U);
}

static uint32_t getSPIClock(SPI_TypeDef *regs)
{
    uint32_t frequency = 0;
#if defined(STM32F0xx) || defined(STM32G0xx)
    /* SPIx source CLK is PCKL1 */
    frequency = HAL_RCC_GetPCLK1Freq();
#else
#if defined(SPI1_BASE)
    if (regs == SPI1) {
#if defined(RCC_PERIPHCLK_SPI1) || defined(RCC_PERIPHCLK_SPI123)
#ifdef RCC_PERIPHCLK_SPI1
        frequency = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI1);
#else
        frequency = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI123);
#endif
        if (frequency == 0)
#endif
        {
            /* SPI1, SPI4, SPI5 and SPI6. Source CLK is PCKL2 */
            frequency = HAL_RCC_GetPCLK2Freq();
        }
    }
#endif // SPI1_BASE
#if defined(SPI2_BASE)
    if (regs == SPI2) {
#if defined(RCC_PERIPHCLK_SPI2) || defined(RCC_PERIPHCLK_SPI123) || defined(RCC_PERIPHCLK_SPI23)
#ifdef RCC_PERIPHCLK_SPI2
        frequency = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI2);
#elif defined(RCC_PERIPHCLK_SPI123)
        frequency = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI123);
#else
        frequency = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI23);
#endif
        if (frequency == 0)
#endif
        {
            /* SPI_2 and SPI_3. Source CLK is PCKL1 */
            frequency = HAL_RCC_GetPCLK1Freq();
        }
    }
#endif // SPI2_BASE
#if defined(SPI3_BASE)
    if (regs == SPI3) {
#if defined(RCC_PERIPHCLK_SPI3) || defined(RCC_PERIPHCLK_SPI123) || defined(RCC_PERIPHCLK_SPI23)
#ifdef RCC_PERIPHCLK_SPI3
        frequency = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3);
#elif defined(RCC_PERIPHCLK_SPI123)
        frequency = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI123);
#else
        frequency = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI23);
#endif
        if (frequency == 0)
#endif
        {
            /* SPI_2 and SPI_3. Source CLK is PCKL1 */
            frequency = HAL_RCC_GetPCLK1Freq();
        }
    }
#endif // SPI3_BASE
#if defined(SPI4_BASE)
    if (regs == SPI4) {
#if defined(RCC_PERIPHCLK_SPI4) || defined(RCC_PERIPHCLK_SPI45)
#ifdef RCC_PERIPHCLK_SPI4
        frequency = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI4);
#else
        frequency = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI45);
#endif
        if (frequency == 0)
#endif
        {
            /* SPI1, SPI4, SPI5 and SPI6. Source CLK is PCKL2 */
            frequency = HAL_RCC_GetPCLK2Freq();
        }
    }
#endif // SPI4_BASE
#if defined(SPI5_BASE)
    if (regs == SPI5) {
#if defined(RCC_PERIPHCLK_SPI5) || defined(RCC_PERIPHCLK_SPI45)
#ifdef RCC_PERIPHCLK_SPI5
        frequency = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI5);
#else
        frequency = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI45);
#endif
        if (frequency == 0)
#endif
        {
            /* SPI1, SPI4, SPI5 and SPI6. Source CLK is PCKL2 */
            frequency = HAL_RCC_GetPCLK2Freq();
        }
    }
#endif // SPI5_BASE
#if defined(SPI6_BASE)
    if (regs == SPI6) {
#if defined(RCC_PERIPHCLK_SPI6)
        frequency = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI6);
        if (frequency == 0)
#endif
        {
            /* SPI1, SPI4, SPI5 and SPI6. Source CLK is PCKL2 */
            frequency = HAL_RCC_GetPCLK2Freq();
        }
    }
#endif // SPI6_BASE
#endif

    return frequency;
}

#define SPI_DEFAULT_TIMEOUT 100U

Status SPI_Driver::configure(SPI_Mode mode, SPI_Config config)
{
    if (_regs == NULL) {
        return Status::ERROR;
    }

    _mode = mode;
    _config = config;

    // Enable SPI clock
#if defined(SPI1_BASE)
    if (_regs == SPI1) {
        __HAL_RCC_SPI1_CLK_ENABLE();
        __HAL_RCC_SPI1_FORCE_RESET();
        __HAL_RCC_SPI1_RELEASE_RESET();
    }
#endif

#if defined(SPI2_BASE)
    if (_regs == SPI2) {
        __HAL_RCC_SPI2_CLK_ENABLE();
        __HAL_RCC_SPI2_FORCE_RESET();
        __HAL_RCC_SPI2_RELEASE_RESET();
    }
#endif

#if defined(SPI3_BASE)
    if (_regs == SPI3) {
        __HAL_RCC_SPI3_CLK_ENABLE();
        __HAL_RCC_SPI3_FORCE_RESET();
        __HAL_RCC_SPI3_RELEASE_RESET();
    }
#endif

#if defined(SPI4_BASE)
    if (_regs == SPI4) {
        __HAL_RCC_SPI4_CLK_ENABLE();
        __HAL_RCC_SPI4_FORCE_RESET();
        __HAL_RCC_SPI4_RELEASE_RESET();
    }
#endif

#if defined(SPI5_BASE)
    if (_regs == SPI5) {
        __HAL_RCC_SPI5_CLK_ENABLE();
        __HAL_RCC_SPI5_FORCE_RESET();
        __HAL_RCC_SPI5_RELEASE_RESET();
    }
#endif

#if defined(SPI6_BASE)
    if (_regs == SPI6) {
        __HAL_RCC_SPI6_CLK_ENABLE();
        __HAL_RCC_SPI6_FORCE_RESET();
        __HAL_RCC_SPI6_RELEASE_RESET();
    }
#endif

    _state = SPI_State::BUSY;

    CLEAR_BIT(_regs->CR1, SPI_CR1_SPE);

    uint16_t CR1 = static_cast<uint16_t>(_mode)
                 | static_cast<uint16_t>(_config._busLine)
                 | static_cast<uint16_t>(_config._busMode)
                 | static_cast<uint16_t>(_config._bitOrder)
                 | SPI_DATASIZE_8BIT           //<-- force always
                 | SPI_NSS_SOFT                //<-- master: sw select / slave: hard input
                 | SPI_TIMODE_DISABLE          //<-- force always
                 | SPI_CRCCALCULATION_DISABLE; //<-- force always

    if (_mode == SPI_Mode::MASTER) {
        CR1 |= SPI_NSS_SOFT;

        uint32_t spi_clock = getSPIClock(_regs);
        if (_config._clock >= (spi_clock / 2)) {
            CR1 |= SPI_BAUDRATEPRESCALER_2;
        }
        else if (_config._clock >= (spi_clock / 4)) {
            CR1 |= SPI_BAUDRATEPRESCALER_4;
        }
        else if (_config._clock >= (spi_clock / 8)) {
            CR1 |= SPI_BAUDRATEPRESCALER_8;
        }
        else if (_config._clock >= (spi_clock / 16)) {
            CR1 |= SPI_BAUDRATEPRESCALER_16;
        }
        else if (_config._clock >= (spi_clock / 32)) {
            CR1 |= SPI_BAUDRATEPRESCALER_32;
        }
        else if (_config._clock >= (spi_clock / 64)) {
            CR1 |= SPI_BAUDRATEPRESCALER_64;
        }
        else if (_config._clock >= (spi_clock / 128)) {
            CR1 |= SPI_BAUDRATEPRESCALER_128;
        }
        else {
            CR1 |= SPI_BAUDRATEPRESCALER_256;
        }
    } else {
        CR1 |= SPI_BAUDRATEPRESCALER_2;
    }

    WRITE_REG(_regs->CR1, CR1);
    WRITE_REG(_regs->CR2, 0x0U);//TODO if configured as slave - here need enable interrupts?, need readme for how to use

    SET_BIT(_regs->CR1, SPI_CR1_SPE);

    _errors = HAL_SPI_ERROR_NONE;
    _state = SPI_State::READY;

    return Status::OK;
}

Status SPI_Driver::send(uint8_t *data, uint16_t size)
{
    if (_state != SPI_State::READY) {
        return Status::BUSY;
    }

    if (data == NULL || size == 0U) {
        return Status::ERROR;
    }

    _state = SPI_State::BUSY_TX;
    _errors = HAL_SPI_ERROR_NONE;

    _txDataBuf = (uint8_t *) data;
    _txDataLen = size;
    _txDataCnt = size;
    // hspi->TxISR = NULL;

    _rxDataBuf = (uint8_t *) NULL;
    _rxDataLen = 0;
    _rxDataCnt = 0;
    // hspi->RxISR = NULL;

    if (_config._busLine == SPI_BusLine::ONE_LINE_BIDIR) {
        CLEAR_BIT(_regs->CR1, SPI_CR1_SPE);
        SET_BIT(_regs->CR1, SPI_CR1_BIDIOE);
    }

    if ((_regs->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
        SET_BIT(_regs->CR1, SPI_CR1_SPE);
    }

    if (_mode == SPI_Mode::SLAVE || size == 0x01U) {
        *(__IO uint8_t *) &_regs->DR = *_txDataBuf;
        _txDataBuf++;
        _txDataCnt--;
    }

    while (_txDataCnt > 0U) {
        if ((_regs->SR & SPI_FLAG_TXE) == SPI_FLAG_TXE) {
            *(__IO uint8_t *) &_regs->DR = *_txDataBuf;
            _txDataBuf++;
            _txDataCnt--;
        }
    }

    while ((_regs->SR & SPI_FLAG_BSY) != 0U);

    /* Clear overrun flag in 2 Lines communication mode because received is not read */
    if (_config._busLine == SPI_BusLine::TWO_LINE) {
        _SPI_CLEAR_OVR(_regs);
    }

    _state = SPI_State::READY;
    return Status::OK;
}

Status SPI_Driver::send_IRQ(uint8_t *data, uint16_t size)
{
    if (_state != SPI_State::READY) {
        return Status::BUSY;
    }

    if (data == NULL || size == 0U) {
        return Status::ERROR;
    }

    _state = SPI_State::BUSY_TX;
    _errors = HAL_SPI_ERROR_NONE;

    _txDataBuf = data;
    _txDataLen = size;
    _txDataCnt = size;
    // hspi->TxISR = SPI_TxISR_8BIT;

    _rxDataBuf = (uint8_t *) NULL;
    _rxDataLen = 0;
    _rxDataCnt = 0;
    // hspi->RxISR = NULL;

    if (_config._busLine == SPI_BusLine::ONE_LINE_BIDIR) {
        CLEAR_BIT(_regs->CR1, SPI_CR1_SPE);
        SET_BIT(_regs->CR1, SPI_CR1_BIDIOE);
    }

    SET_BIT(_regs->CR2, (SPI_IT_TXE | SPI_IT_ERR));

    if ((_regs->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
        SET_BIT(_regs->CR1, SPI_CR1_SPE);
    }

    return Status::OK;
}

Status SPI_Driver::recv(uint8_t *data, uint16_t size)
{
    if (_mode == SPI_Mode::MASTER && _config._busLine == SPI_BusLine::TWO_LINE) {
        _state = SPI_State::BUSY_RX;
        return transfer(data, data, size);
    }

    if (_state != SPI_State::READY) {
        return Status::BUSY;
    }

    if (data == NULL || size == 0U) {
        return Status::ERROR;
    }

    _state = SPI_State::BUSY_RX;
    _errors = HAL_SPI_ERROR_NONE;

    _txDataBuf = (uint8_t *) NULL;
    _txDataLen = 0;
    _txDataCnt = 0;
    // hspi->TxISR = NULL;

    _rxDataBuf = (uint8_t *) data;
    _rxDataLen = size;
    _rxDataCnt = size;
    // hspi->RxISR = NULL;

    if (_config._busLine == SPI_BusLine::ONE_LINE_BIDIR) {
        CLEAR_BIT(_regs->CR1, SPI_CR1_SPE);
        CLEAR_BIT(_regs->CR1, SPI_CR1_BIDIMODE);
    }

    if ((_regs->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
        SET_BIT(_regs->CR1, SPI_CR1_SPE);
    }

    while (_rxDataCnt > 0U) {
        if ((_regs->SR & SPI_FLAG_RXNE) == SPI_FLAG_RXNE) {
            *_rxDataBuf = *(__IO uint8_t *) &_regs->DR;
            _rxDataBuf++;
            _rxDataCnt--;
        }
        else {
            /* TODO maybe: Timeout management */
        }
    }

    // /* Check the end of the transaction */
    // if (SPI_EndRxTransaction(hspi, Timeout, tickstart) != HAL_OK) {
    //     _errors = HAL_SPI_ERROR_FLAG;
    //     return Status::ERROR;
    // }

    _state = SPI_State::READY;
    return Status::OK;
}

Status SPI_Driver::recv_IRQ(uint8_t *data, uint16_t size)
{
    if (_mode == SPI_Mode::MASTER && _config._busLine == SPI_BusLine::TWO_LINE) {
        _state = SPI_State::BUSY_RX;
        return transfer_IRQ(data, data, size);
    }

    if (data == NULL || size == 0U) {
        return Status::ERROR;
    }

    if (_state != SPI_State::READY) {
        return Status::BUSY;
    }

    _state = SPI_State::BUSY_RX;
    _errors = HAL_SPI_ERROR_NONE;

    _txDataBuf = (uint8_t *) NULL;
    _txDataLen = 0;
    _txDataCnt = 0;
    // hspi->TxISR = NULL;

    _rxDataBuf = (uint8_t *) data;
    _rxDataLen = size;
    _rxDataCnt = size;
    // hspi->RxISR = SPI_RxISR_8BIT;

    if (_config._busLine == SPI_BusLine::ONE_LINE_BIDIR) {
        CLEAR_BIT(_regs->CR1, SPI_CR1_SPE);
        CLEAR_BIT(_regs->CR1, SPI_CR1_BIDIMODE);
    }

    SET_BIT(_regs->CR2, SPI_IT_RXNE | SPI_IT_ERR);

    if ((_regs->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
        SET_BIT(_regs->CR1, SPI_CR1_SPE);
    }

    return Status::OK;
}

Status SPI_Driver::transfer(uint8_t *txData, uint8_t *rxData, uint16_t size)
{
    if (!(
        _state == SPI_State::READY ||
        (_state == SPI_State::BUSY_RX && _mode == SPI_Mode::MASTER && _config._busLine == SPI_BusLine::TWO_LINE)
    )) {
        return Status::BUSY;
    }

    if (txData == NULL || rxData == NULL || size == 0U) {
        return Status::ERROR;
    }

    if (_state != SPI_State::BUSY_RX) {
        _state = SPI_State::BUSY_TX_RX;
    }

    _errors = HAL_SPI_ERROR_NONE;

    _txDataBuf = (uint8_t *) txData;
    _txDataLen = size;
    _txDataCnt = size;
    // hspi->TxISR = NULL;

    _rxDataBuf = (uint8_t *) rxData;
    _rxDataLen = size;
    _rxDataCnt = size;
    // hspi->RxISR = NULL;

    if ((_regs->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
        SET_BIT(_regs->CR1, SPI_CR1_SPE);
    }

    if (_mode == SPI_Mode::SLAVE || size == 1U) {
        *((__IO uint8_t *) &_regs->DR) = *_txDataBuf;
        _txDataBuf++;
        _txDataCnt--;
    }

    bool txAllowed = true;

    while (_txDataCnt > 0U || _rxDataCnt > 0) {
        if ((_regs->SR & SPI_FLAG_TXE) == SPI_FLAG_TXE && _txDataCnt > 0 && txAllowed) {
            *(__IO uint8_t *) &_regs->DR = *_txDataBuf;
            _txDataBuf++;
            _txDataCnt--;
            txAllowed = false;
        }
        if ((_regs->SR & SPI_FLAG_RXNE) == SPI_FLAG_RXNE && _rxDataCnt > 0) {
            *_rxDataBuf = *(__IO uint8_t *) &_regs->DR;
            _rxDataBuf++;
            _rxDataCnt--;
            txAllowed = true;
        }
        //TODO maybe timeout
    }

    while ((_regs->SR & SPI_FLAG_BSY) != 0U);

    if (_config._busLine == SPI_BusLine::TWO_LINE) {
        _SPI_CLEAR_OVR(_regs);
    }

    _state = SPI_State::READY;
    return Status::OK;
}

Status SPI_Driver::transfer_IRQ(uint8_t *txData, uint8_t *rxData, uint16_t size)
{
    if (!(
        _state == SPI_State::READY ||
        (_state == SPI_State::BUSY_RX && _mode == SPI_Mode::MASTER && _config._busLine == SPI_BusLine::TWO_LINE)
    )) {
        return Status::BUSY;
    }

    if (txData == NULL || rxData == NULL || size == 0U) {
        return Status::ERROR;
    }

    if (_state != SPI_State::BUSY_RX) {
        _state = SPI_State::BUSY_TX_RX;
    }

    _errors = HAL_SPI_ERROR_NONE;

    _txDataBuf = (uint8_t *) txData;
    _txDataLen = size;
    _txDataCnt = size;
    // hspi->TxISR = SPI_2linesTxISR_8BIT;

    _rxDataBuf = (uint8_t *) rxData;
    _rxDataLen = size;
    _rxDataCnt = size;
    // hspi->RxISR = SPI_2linesRxISR_8BIT;

    SET_BIT(_regs->CR2, (SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR));

    if ((_regs->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
        SET_BIT(_regs->CR1, SPI_CR1_SPE);
    }

    return Status::OK;
}

//TODO all copied HAL functions simplify as possible for resolve common logic
static HAL_StatusTypeDef SPI_EndRxTransaction(SPI_HandleTypeDef *hspi,  uint32_t Timeout, uint32_t Tickstart)
{
    if (
        hspi->Init.Mode == SPI_MODE_MASTER
        && (hspi->Init.Direction == SPI_DIRECTION_1LINE || hspi->Init.Direction == SPI_DIRECTION_2LINES_RXONLY)
    ) {
        __HAL_SPI_DISABLE(hspi);
    }

    if (hspi->Init.Mode == SPI_MODE_MASTER && hspi->Init.Direction == SPI_DIRECTION_2LINES_RXONLY) {
        if (SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_RXNE, RESET, Timeout, Tickstart) != HAL_OK) {
            SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
            return HAL_TIMEOUT;
        }
    }
    else {
        if (SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_BSY, RESET, Timeout, Tickstart) != HAL_OK) {
            SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
            return HAL_TIMEOUT;
        }
    }
    return HAL_OK;
}

static HAL_StatusTypeDef SPI_EndRxTxTransaction(SPI_HandleTypeDef *hspi, uint32_t Timeout, uint32_t Tickstart)
{
    if (SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_BSY, RESET, Timeout, Tickstart) != HAL_OK) {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
        return HAL_TIMEOUT;
    }
    return HAL_OK;
}

static void SPI_TxISR_8BIT(struct __SPI_HandleTypeDef *hspi)
{
    *(__IO uint8_t *)&hspi->Instance->DR = (*hspi->pTxBuffPtr);
    hspi->pTxBuffPtr++;
    hspi->TxXferCount--;

    if (hspi->TxXferCount == 0U) {
        SPI_CloseTx_ISR(hspi);
    }
}

static void SPI_2linesTxISR_8BIT(struct __SPI_HandleTypeDef *hspi)
{
    *(__IO uint8_t *)&hspi->Instance->DR = (*hspi->pTxBuffPtr);
    hspi->pTxBuffPtr++;
    hspi->TxXferCount--;

    if (hspi->TxXferCount == 0U) {
        __HAL_SPI_DISABLE_IT(hspi, SPI_IT_TXE);
        if (hspi->RxXferCount == 0U) {
            SPI_CloseRxTx_ISR(hspi);
        }
    }
}

static void SPI_RxISR_8BIT(struct __SPI_HandleTypeDef *hspi)
{
    *hspi->pRxBuffPtr = (*(__IO uint8_t *)&hspi->Instance->DR);
    hspi->pRxBuffPtr++;
    hspi->RxXferCount--;

    if (hspi->RxXferCount == 0U) {
        SPI_CloseRx_ISR(hspi);
    }
}

static void SPI_2linesRxISR_8BIT(struct __SPI_HandleTypeDef *hspi)
{
    *hspi->pRxBuffPtr = *((__IO uint8_t *)&hspi->Instance->DR);
    hspi->pRxBuffPtr++;
    hspi->RxXferCount--;

    if (hspi->RxXferCount == 0U) {
        __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_RXNE | SPI_IT_ERR));
        if (hspi->TxXferCount == 0U) {
            SPI_CloseRxTx_ISR(hspi);
        }
    }
}

static void SPI_CloseRxTx_ISR(SPI_HandleTypeDef *hspi)
{
    __IO uint32_t count = SPI_DEFAULT_TIMEOUT * (SystemCoreClock / 24U / 1000U);

    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_ERR);

    do {
        if (count == 0U) {
            SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
            break;
        }
        count--;
    } while ((hspi->Instance->SR & SPI_FLAG_TXE) == RESET);

    if (SPI_EndRxTxTransaction(hspi, SPI_DEFAULT_TIMEOUT, HAL_GetTick()) != HAL_OK) {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
    }

    if (hspi->Init.Direction == SPI_DIRECTION_2LINES) {
        __HAL_SPI_CLEAR_OVRFLAG(hspi);
    }

    if (hspi->ErrorCode == HAL_SPI_ERROR_NONE) {
        if (hspi->State == HAL_SPI_STATE_BUSY_RX) {
            hspi->State = HAL_SPI_STATE_READY;
            HAL_SPI_RxCpltCallback(hspi);
        }
        else {
            hspi->State = HAL_SPI_STATE_READY;
            HAL_SPI_TxRxCpltCallback(hspi);
        }
    }
    else {
      hspi->State = HAL_SPI_STATE_READY;
      HAL_SPI_ErrorCallback(hspi);
    }
}

static void SPI_CloseRx_ISR(SPI_HandleTypeDef *hspi)
{
    __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_RXNE | SPI_IT_ERR));

    if (SPI_EndRxTransaction(hspi, SPI_DEFAULT_TIMEOUT, HAL_GetTick()) != HAL_OK) {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
    }

    if (hspi->Init.Direction == SPI_DIRECTION_2LINES) {
        __HAL_SPI_CLEAR_OVRFLAG(hspi);
    }
    hspi->State = HAL_SPI_STATE_READY;

    if (hspi->ErrorCode == HAL_SPI_ERROR_NONE) {
        HAL_SPI_RxCpltCallback(hspi);
    }
    else {
        HAL_SPI_ErrorCallback(hspi);
    }
}

static void SPI_CloseTx_ISR(SPI_HandleTypeDef *hspi)
{
    __IO uint32_t count = SPI_DEFAULT_TIMEOUT * (SystemCoreClock / 24U / 1000U);

    do {
        if (count == 0U) {
            SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
            break;
        }
        count--;
    } while ((hspi->Instance->SR & SPI_FLAG_TXE) == RESET);

    __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_TXE | SPI_IT_ERR));

    if (SPI_EndRxTxTransaction(hspi, SPI_DEFAULT_TIMEOUT, HAL_GetTick()) != HAL_OK) {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
    }

    if (hspi->Init.Direction == SPI_DIRECTION_2LINES) {
        __HAL_SPI_CLEAR_OVRFLAG(hspi);
    }

    hspi->State = HAL_SPI_STATE_READY;
    if (hspi->ErrorCode != HAL_SPI_ERROR_NONE) {
        HAL_SPI_ErrorCallback(hspi);
    }
    else {
        HAL_SPI_TxCpltCallback(hspi);
    }
}

void _SPI_ISRHandler(SPI_TypeDef *regs)
{
    //TODO rethink ISR handler, make it independent from interrupt enabled
    //TODO check counters on flags set instead of wait for on counter is 0
}

void SPI_Driver::dispatchIRQ(void)
{
    uint32_t CR2 = _regs->CR2;
    uint32_t SR  = _regs->SR;

    if ((SR & SPI_FLAG_OVR) == 0U && (SR & SPI_FLAG_RXNE) != 0U && (CR2 & SPI_IT_RXNE) != 0U) {
        //hspi->RxISR(hspi); SPI_RxISR_8BIT || SPI_2linesRxISR_8BIT
        return;
    }

    if ((SR & SPI_FLAG_TXE) != 0U && (CR2 & SPI_IT_TXE) != 0U) {
        //hspi->TxISR(hspi); SPI_TxISR_8BIT || SPI_2linesTxISR_8BIT
        return;
    }

    if ((SR & SPI_FLAG_MODF) != 0U || (SR & SPI_FLAG_OVR) != 0U && (CR2 & SPI_IT_ERR) != 0U) {
        if ((SR & SPI_FLAG_OVR) != 0U) {
            if (_state != SPI_State::BUSY_TX) {
                SET_BIT(_errors, HAL_SPI_ERROR_OVR);
                _SPI_CLEAR_OVR(_regs);
            }
            else {
                _SPI_CLEAR_OVR(_regs);
                return;
            }
        }

        if ((SR & SPI_FLAG_MODF) != 0U) {
            SET_BIT(_errors, HAL_SPI_ERROR_MODF);
            _SPI_CLEAR_MODF(_regs);
        }

        if (_errors != HAL_SPI_ERROR_NONE) {
            CLEAR_BIT(_regs->CR2, SPI_IT_RXNE | SPI_IT_TXE | SPI_IT_ERR);
            _state = SPI_State::READY;

            if ((CR2 & SPI_CR2_TXDMAEN) != 0U || (CR2 & SPI_CR2_RXDMAEN) != 0U) {
                //TODO DMA abort
            }
            else {
                //TODO on error
            }
        }
    }
}
