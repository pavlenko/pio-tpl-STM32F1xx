#include "USART.hpp"

#ifndef STM32_USART_REGISTER_IRQ_HANDLERS
#define STM32_USART_REGISTER_IRQ_HANDLERS 1
#endif

namespace STM32 {
    template <uint8_t index>
    inline void USART<index>::reset(void)
    {
        if constexpr (index == 1U)
        {
            __HAL_RCC_USART1_FORCE_RESET();
            __HAL_RCC_USART1_RELEASE_RESET();
        }
#if defined(USART2_BASE)
        else if constexpr (index == 2U)
        {
            __HAL_RCC_USART2_FORCE_RESET();
            __HAL_RCC_USART2_RELEASE_RESET();
        }
#endif
#if defined(USART3_BASE)
        else if constexpr (index == 3U)
        {
            __HAL_RCC_USART3_FORCE_RESET();
            __HAL_RCC_USART3_RELEASE_RESET();
        }
#endif
#if defined(USART4_BASE)
        else if constexpr (index == 4U)
        {
            __HAL_RCC_USART4_FORCE_RESET();
            __HAL_RCC_USART4_RELEASE_RESET();
        }
#endif
#if defined(USART5_BASE)
        else if constexpr (index == 5U)
        {
            __HAL_RCC_USART5_FORCE_RESET();
            __HAL_RCC_USART5_RELEASE_RESET();
        }
#endif
#if defined(USART6_BASE)
        else if constexpr (index == 6U)
        {
            __HAL_RCC_USART6_FORCE_RESET();
            __HAL_RCC_USART6_RELEASE_RESET();
        }
#endif
#if defined(USART7_BASE)
        else if constexpr (index == 7U)
        {
            __HAL_RCC_USART7_FORCE_RESET();
            __HAL_RCC_USART7_RELEASE_RESET();
        }
#endif
#if defined(USART8_BASE)
        else if constexpr (index == 8U)
        {
            __HAL_RCC_USART8_FORCE_RESET();
            __HAL_RCC_USART8_RELEASE_RESET();
        }
#endif
        _errors = 0U;
    }

    template <uint8_t index>
    inline void USART<index>::_txIRQ()
    {
        if (_status == USART_Status::BUSY_TX) {
            if (_config._dataBits == USART_DataBits::DATA_9BIT && _config._parity == USART_Parity::NONE)
            {
                _regs->DR = (uint16_t)(*(uint16_t *)_txDataBuf & 0x01FFU);
                _txDataBuf += 2U;
            }
            else
            {
                _regs->DR = (uint8_t)(*_txDataBuf & 0x00FFU);
                _txDataBuf += 1U;
            }

            if (--_txDataCnt == 0U)
            {
                _regs->CR1 &= ~USART_CR1_TXEIE;
                _regs->CR1 |= USART_CR1_TCIE;
            }
        }
    }

    template <uint8_t index>
    inline void USART<index>::dispatchIRQ(void)
    {
        uint32_t SR  = _regs->SR;
        uint32_t CR1 = _regs->CR1;
        uint32_t CR3 = _regs->CR3;
        uint32_t errors = 0x00U;

        /* If no error occurs */
        errors = (SR & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
        if (errors == 0U)
        {
            /* UART in mode Receiver -------------------------------------------------*/
            if ((SR & USART_SR_RXNE) != 0U && (CR1 & USART_CR1_RXNEIE) != 0U)
            {
                //TODO UART_Receive_IT(huart);
                return;
            }
        }
        /* If some errors occur */
        if (errors != 0U && ((CR3 & USART_CR3_EIE) != 0U || (CR1 & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != 0U))
        {
            if ((SR & USART_SR_PE) != 0U && (CR1 & USART_CR1_PEIE) != 0U)
            {
                _errors |= USART_Error::PARITY;
            }
            if ((SR & USART_SR_NE) != 0U && (CR3 & USART_CR3_EIE) != 0U)
            {
                _errors |= USART_Error::NOISE;
            }
            if ((SR & USART_SR_FE) != 0U && (CR3 & USART_CR3_EIE) != 0U)
            {
                _errors |= USART_Error::FRAME;
            }
            if ((SR & USART_SR_ORE) != 0U && ((CR1 & USART_CR1_RXNEIE) != 0U || (CR3 & USART_CR3_EIE) != 0U))
            {
                _errors |= USART_Error::OVERRUN;
            }
            /* Call UART Error Call back function if needed   --------------------------*/
            if (_errors != USART_Error::NONE)
            {
                /* UART in mode Receiver -----------------------------------------------*/
                if ((SR & USART_SR_RXNE) != 0U && (CR1 & USART_CR1_RXNEIE) != 0U)
                {
                    //TODO UART_Receive_IT(huart);//???
                }
                /* If Overrun error occurs, consider error as blocking */
                if ((_errors & USART_Error::OVERRUN) != 0U)
                {
                    /* Blocking error : transfer is aborted
                       Set the UART state ready to be able to start again the process,
                       Disable Rx Interrupts, and disable Rx DMA request, if ongoing */
                    //TODO UART_EndRxTransfer(huart);

                    /* Call user error callback */
                    //TODO HAL_UART_ErrorCallback(huart);
                }
                else
                {
                    /* Non Blocking error : transfer could go on. Error is notified to user through user error callback */
                    //TODO HAL_UART_ErrorCallback(huart);

                    _errors = 0U;
                }
            }
            return;
        }
        /* UART in mode Transmitter ------------------------------------------------*/
        if ((SR & USART_SR_TXE) != 0U && (CR1 & USART_CR1_TXEIE) != 0U)
        {
            _txIRQ();
            return;
        }
        /* UART in mode Transmitter end --------------------------------------------*/
        if ((SR & USART_SR_TC) != 0U && (CR1 & USART_CR1_TCIE) != 0U)
        {
            //TODO UART_EndTransmit_IT(huart);
            return;
        }
    }
}

extern "C"
{
#if (STM32_USART_REGISTER_IRQ_HANDLERS == 1)
#if defined(USART1_BASE)
    void USART1_IRQHandler(void)
    {
        // using namespace STM32;
        // if (USART<1>::errorIRQHandler) {
        //     USART<1>::errorIRQHandler();
        // }
        // if (USART<1>::eventIRQHandler) {
        //     USART<1>::eventIRQHandler();
        // }
        STM32::USART<1>::getInstance().dispatchIRQ();
    }
#endif
#if defined(USART2_BASE)
    void USART2_IRQHandler(void)
    {
        using namespace STM32;
        if (USART<2>::errorIRQHandler) {
            USART<2>::errorIRQHandler();
        }
        if (USART<2>::eventIRQHandler) {
            USART<2>::eventIRQHandler();
        }
    }
#endif
#if defined(USART3_BASE)
    void USART3_IRQHandler(void)
    {
        using namespace STM32;
        if (USART<3>::errorIRQHandler) {
            USART<3>::errorIRQHandler();
        }
        if (USART<3>::eventIRQHandler) {
            USART<3>::eventIRQHandler();
        }
    }
#endif
#if defined(USART4_BASE)
    void USART4_IRQHandler(void)
    {
        using namespace STM32;
        if (USART<4>::errorIRQHandler) {
            USART<4>::errorIRQHandler();
        }
        if (USART<4>::eventIRQHandler) {
            USART<4>::eventIRQHandler();
        }
    }
#endif
#if defined(USART5_BASE)
    void USART5_IRQHandler(void)
    {
        using namespace STM32;
        if (USART<5>::errorIRQHandler) {
            USART<5>::errorIRQHandler();
        }
        if (USART<5>::eventIRQHandler) {
            USART<5>::eventIRQHandler();
        }
    }
#endif
#if defined(USART6_BASE)
    void USART6_IRQHandler(void)
    {
        using namespace STM32;
        if (USART<6>::errorIRQHandler) {
            USART<6>::errorIRQHandler();
        }
        if (USART<6>::eventIRQHandler) {
            USART<6>::eventIRQHandler();
        }
    }
#endif
#if defined(USART7_BASE)
    void USART7_IRQHandler(void)
    {
        using namespace STM32;
        if (USART<7>::errorIRQHandler) {
            USART<7>::errorIRQHandler();
        }
        if (USART<7>::eventIRQHandler) {
            USART<7>::eventIRQHandler();
        }
    }
#endif
#if defined(USART8_BASE)
    void USART8_IRQHandler(void)
    {
        using namespace STM32;
        if (USART<8>::errorIRQHandler) {
            USART<8>::errorIRQHandler();
        }
        if (USART<8>::eventIRQHandler) {
            USART<8>::eventIRQHandler();
        }
    }
#endif
#endif
}