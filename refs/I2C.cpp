#include "I2C.hpp"

#define I2C_MASTER_ADDRESS 0x01U

void I2Cx::init(I2C_speed_t speed)
{
    init(speed, (uint8_t) I2C_MASTER_ADDRESS, false, false);
}

/**** STEPS FOLLOWED  ************
 * https://controllerstech.com/stm32-i2c-configuration-using-registers/
1. Enable the I2C CLOCK and GPIO CLOCK
2. Configure the I2C PINs for ALternate Functions
	a) Select Alternate Function in MODER Register
	b) Select Open Drain Output
	c) Select High SPEED for the PINs
	d) Select Pull-up for both the Pins
	e) Configure the Alternate Function in AFR Register
3. Reset the I2C
4. Program the peripheral input clock in I2C_CR2 Register in order to generate correct timings
5. Configure the clock control registers
6. Configure the rise time register
7. Program the I2C_CR1 register to enable the peripheral
*/
void I2Cx::init(I2C_speed_t speed, uint8_t ownAddress, bool generalCall, bool noStrech)
{
    IRQn_Type irqEV, irqER;

    this->_state = I2C_STATE_BUSY;

#if defined(I2C1_BASE)
    if (this->_regs == I2C1) {
        __HAL_RCC_I2C1_CLK_ENABLE();
        __HAL_RCC_I2C1_FORCE_RESET();
        __HAL_RCC_I2C1_RELEASE_RESET();
        irqEV = I2C1_EV_IRQn;
        irqER = I2C1_ER_IRQn;
    }
#endif
#if defined(I2C2_BASE)
    if (this->_regs == I2C2) {
        __HAL_RCC_I2C2_CLK_ENABLE();
        __HAL_RCC_I2C2_FORCE_RESET();
        __HAL_RCC_I2C2_RELEASE_RESET();
        irqEV = I2C2_EV_IRQn;
        irqER = I2C2_ER_IRQn;
    }
#endif
#if defined(I2C3_BASE)
    if (this->regs == I2C3) {
        __HAL_RCC_I2C3_CLK_ENABLE();
        __HAL_RCC_I2C3_FORCE_RESET();
        __HAL_RCC_I2C3_RELEASE_RESET();
        irqEV = I2C3_EV_IRQn;
        irqER = I2C3_ER_IRQn;
    }
#endif
#if defined(I2C4_BASE)
    if (this->regs == I2C4) {
        __HAL_RCC_I2C4_CLK_ENABLE();
        __HAL_RCC_I2C4_FORCE_RESET();
        __HAL_RCC_I2C4_RELEASE_RESET();
        irqEV = I2C4_EV_IRQn;
        irqER = I2C4_ER_IRQn;
    }
#endif

    HAL_NVIC_SetPriority(irqEV, I2C_IRQ_PRIORITY, I2C_IRQ_SUBPRIORITY);
    HAL_NVIC_EnableIRQ(irqEV);
    HAL_NVIC_SetPriority(irqER, I2C_IRQ_PRIORITY, I2C_IRQ_SUBPRIORITY);
    HAL_NVIC_EnableIRQ(irqER);

    this->_regs->CR1 &= ~I2C_CR1_PE; // <-- disable I2C before change configs

    this->_regs->CR1 |= I2C_CR1_SWRST; // <-- Reset I2C
    this->_regs->CR1 &= ~I2C_CR1_SWRST;

    /* Get PCLK1 frequency */
    uint32_t PCLK1 = (SystemCoreClock >> APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos]);

    /* Calculate frequency range */
    uint32_t FREQ = PCLK1 / 1000000;

    /* Configure I2Cx: Frequency range */
    MODIFY_REG(this->_regs->CR2, I2C_CR2_FREQ, FREQ);

    /* Configure I2Cx: Rise Time */
    MODIFY_REG(this->_regs->TRISE, I2C_TRISE_TRISE, I2C_RISE_TIME(FREQ, speed));

    /* Configure I2Cx: Speed */
    if (PCLK1 <= 100000U) {
        MODIFY_REG(this->_regs->CCR, (I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR), I2C_SPEED_STANDARD(PCLK1, speed));
    } else {
        MODIFY_REG(
            this->_regs->CCR,
            (I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR),
            (I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CALCULATION(PCLK1, speed, 25U))
        );
    }

    /* Configure I2Cx: Generalcall and NoStretch mode */
    MODIFY_REG(
        this->_regs->CR1,
        (I2C_CR1_ENGC | I2C_CR1_NOSTRETCH),
        (generalCall << I2C_CR1_ENGC_Pos | noStrech << I2C_CR1_NOSTRETCH_Pos)
    );

    /* Configure I2Cx: Own Address1 and addressing mode */
    MODIFY_REG(
        this->_regs->OAR1,
        (I2C_OAR1_ADDMODE | I2C_OAR1_ADD8_9 | I2C_OAR1_ADD1_7 | I2C_OAR1_ADD0),
        (0x00004000U /* 7-bit */ | ownAddress << 1)
    );

    /* Configure I2Cx: Dual mode and Own Address2 */
    MODIFY_REG(
        this->_regs->OAR2,
        (I2C_OAR2_ENDUAL | I2C_OAR2_ADD2),
        (0 /* disabled */ | 0 /* no second address */)
    );

    this->_regs->CR1 |= I2C_CR1_PE; // <-- enable I2C after apply configs

    this->_state = I2C_STATE_READY;
    this->_errors = I2C_ERROR_NONE;
}

void I2Cx::masterSend_IT(uint16_t devAddress, uint8_t *buf, uint16_t len)
{
    if (this->_state != I2C_STATE_READY) { // <-- If not ready just return for now
        return;
    }

    if ((this->_regs->CR1 & I2C_CR1_PE) != I2C_CR1_PE) { // <-- enable I2C if not already enabled
        this->_regs->CR1 |= I2C_CR1_PE;
    }

    this->_regs->CR1 &= ~I2C_CR1_POS; // <-- Disable POS

    /* Prepare transfer parameters */
    this->_state      = I2C_STATE_BUSY_TX;
    this->_errors     = I2C_ERROR_NONE;
    this->_devAddress = devAddress;
    this->_dataBuf    = buf;
    this->_dataLen    = len;
    this->_dataCnt    = len;

    this->_regs->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN; // <-- Enable interrupts
    this->_regs->CR1 |= I2C_CR1_START; // <-- Generate START
}

void I2Cx::masterRecv_IT(uint16_t devAddress, uint8_t *buf, uint16_t len)
{
    if (this->_state != I2C_STATE_READY) { // <-- If not ready just return for now
        return;
    }

    if ((this->_regs->CR1 & I2C_CR1_PE) != I2C_CR1_PE) { // <-- enable I2C if not already enabled
        this->_regs->CR1 |= I2C_CR1_PE;
    }

    this->_regs->CR1 &= ~I2C_CR1_POS; // <-- Disable POS

    /* Prepare transfer parameters */
    this->_state      = I2C_STATE_BUSY_RX;
    this->_errors     = I2C_ERROR_NONE;
    this->_devAddress = devAddress;
    this->_dataBuf    = buf;
    this->_dataLen    = len;
    this->_dataCnt    = len;

    this->_regs->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN; // <-- Enable interrupts
    this->_regs->CR2 |= I2C_CR1_ACK; // <-- Enable ACK
    this->_regs->CR1 |= I2C_CR1_START; // <-- Generate START
}

void I2Cx::slaveSend_IT(uint8_t *buf, uint16_t len)
{
    //TODO check buf & len values???

    if ((this->_regs->CR1 & I2C_CR1_PE) != I2C_CR1_PE) { // <-- enable I2C if not already enabled
        this->_regs->CR1 |= I2C_CR1_PE;
    }

    this->_regs->CR1 &= ~I2C_CR1_POS; // <-- Disable POS

    /* Prepare transfer parameters */
    this->_state      = I2C_STATE_BUSY_TX;
    this->_errors     = I2C_ERROR_NONE;
    this->_dataBuf    = buf;
    this->_dataLen    = len;
    this->_dataCnt    = len;

    this->_regs->CR2 |= I2C_CR1_ACK; // <-- Enable ACK
    this->_regs->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN; // <-- Enable interrupts
}

void I2Cx::slaveRecv_IT(uint8_t *buf, uint16_t len)
{
    //TODO check buf & len values???

    if ((this->_regs->CR1 & I2C_CR1_PE) != I2C_CR1_PE) { // <-- enable I2C if not already enabled
        this->_regs->CR1 |= I2C_CR1_PE;
    }

    this->_regs->CR1 &= ~I2C_CR1_POS; // <-- Disable POS

    /* Prepare transfer parameters */
    this->_state      = I2C_STATE_BUSY_RX;
    this->_errors     = I2C_ERROR_NONE;
    this->_dataBuf    = buf;
    this->_dataLen    = len;
    this->_dataCnt    = len;

    this->_regs->CR2 |= I2C_CR1_ACK; // <-- Enable ACK
    this->_regs->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN; // <-- Enable interrupts
}

void I2Cx::onAddress(cbAddress_t cb)
{
    this->_onAddress = cb;
}

void I2Cx::onReceive(cbReceive_t cb)
{
    this->_onReceive = cb;
}

void I2Cx::onRequest(cbRequest_t cb)
{
    this->_onRequest = cb;
}

void I2Cx::onError(cbError_t cb)
{
    this->_onError = cb;
}

void I2Cx_dispatchEvent(I2Cx &i2c)
{
    __IO uint32_t SR1 = 0;
    __IO uint32_t SR2 = 0;

    /* Read the I2C SR1 and SR2 status registers */
    SR1 = i2c._regs->SR1;
    SR2 = i2c._regs->SR2;

    /* If I2C is slave (MSL flag = 0) */
    if ((SR2 & I2C_SR2_MSL) != I2C_SR2_MSL) {
        /* If ADDR = 1: EV1 */
        if ((SR1 & I2C_SR1_ADDR) == I2C_SR1_ADDR) {
            if (i2c._onAddress) {
                i2c._onAddress(i2c, (SR2 & I2C_SR2_TRA) == RESET);
            }
            /* Clear SR1Register and SR2Register variables to prepare for next IT */
            SR1 = 0;
            SR2 = 0;
         }
        /* If TXE = 1: EV3 */
        if ((SR1 & I2C_SR1_TXE) == I2C_SR1_TXE) {
            /* Write data in data register */
            i2c._regs->DR = *i2c._dataBuf;
            i2c._dataBuf++;
            i2c._dataCnt--;

            SR1 = 0;
            SR2 = 0;
        }
        /* If RXNE = 1: EV2 */
        if ((SR1 & I2C_SR1_RXNE) == I2C_SR1_RXNE) {
            /* Read data from data register */
            *i2c._dataBuf = i2c._regs->DR;
            i2c._dataBuf++;
            i2c._dataCnt--;

            SR1 = 0;
            SR2 = 0;
        }
        /* If STOPF =1: EV4 (Slave has detected a STOP condition on the bus */
        if ((SR1 & I2C_SR1_STOPF) == I2C_SR1_STOPF) {
            i2c._regs->CR1 |= I2C_CR1_PE;
            SR1 = 0;
            SR2 = 0;
        }
    } /* End slave mode */

    /* If SB = 1, I2C master sent a START on the bus: EV5) */
    if ((SR2 & I2C_SR2_MSL) == I2C_SR2_MSL) {
        /* Send the slave address for transmssion or for reception */
        if (i2c._state == I2C_STATE_BUSY_TX) {
            i2c._regs->DR = (uint8_t)((i2c._devAddress << 1) & 0xFE);
        } else {
            i2c._regs->DR = (uint8_t)((i2c._devAddress << 1) | 0x01);
        }
        SR1 = 0;
        SR2 = 0;
    }

    /* If I2C1 is Master (MSL flag = 1) */
    if ((SR2 & I2C_SR2_MSL) == I2C_SR2_MSL) {
        /* If ADDR = 1, EV6 */
        if ((SR1 & I2C_SR1_ADDR) == I2C_SR1_ADDR) {
            /* Write the first data in case the Master is Transmitter */
            if (i2c._state == I2C_STATE_BUSY_TX) {
                /* Write the first data in the data register */
                i2c._regs->DR = *i2c._dataBuf;
                i2c._dataBuf++;
                i2c._dataCnt--;
                /* If no further data to be sent, disable the I2C BUF IT in order to not have a TxE  interrupt */
                if (i2c._dataCnt == 0) {
                    i2c._regs->CR2 &= ~I2C_CR2_ITBUFEN;
                }
            }
            /* Master Receiver */
            else {
                /* At this stage, ADDR is cleared because both SR1 and SR2 were read.*/
                /* EV6_1: used for single byte reception. The ACK disable and the STOP
                Programming should be done just after ADDR is cleared. */
                if (i2c._dataCnt == 1) {
                    /* Clear ACK */
                    i2c._regs->CR1 &= ~I2C_CR1_ACK;
                    /* Program the STOP */
                    i2c._regs->CR1 |= I2C_CR1_STOP;
                }
            }
            SR1 = 0;
            SR2 = 0;
        }
        /* Master transmits the remaing data: from data2 until the last one.  */
        /* If TXE is set */
        if ((SR1 & (I2C_SR1_TXE|I2C_SR1_BTF)) == I2C_SR1_TXE) {
            /* If there is still data to write */
            if (i2c._dataCnt != 0) {
                /* Write the data in DR register */
                i2c._regs->DR = *i2c._dataBuf;
                i2c._dataBuf++;
                i2c._dataCnt--;
                /* If  no data remains to write, disable the BUF IT in order to not have again a TxE interrupt. */
                if (i2c._dataCnt == 0) {
                    /* Disable the BUF IT */
                    i2c._regs->CR2 &= ~I2C_CR2_ITBUFEN;
                }
            }
            SR1 = 0;
            SR2 = 0;
        }
        /* If BTF and TXE are set (EV8_2), program the STOP */
        if ((SR1 & (I2C_SR1_TXE|I2C_SR1_BTF)) == (I2C_SR1_TXE|I2C_SR1_BTF)) {
            /* Program the STOP */
            i2c._regs->CR1 |= I2C_CR1_STOP;
            /* Disable EVT IT In order to not have again a BTF IT */
            i2c._regs->CR2 &= ~I2C_CR2_ITEVTEN;
            SR1 = 0;
            SR2 = 0;
        }
        /* If RXNE is set */
        if ((SR1 & I2C_SR1_RXNE) == I2C_SR1_RXNE) {
            /* Read the data register */
            *i2c._dataBuf = i2c._regs->DR;
            i2c._dataBuf++;
            i2c._dataCnt--;
            /* If it remains only one byte to read, disable ACK and program the STOP (EV7_1) */
            if (i2c._dataCnt == 1) {
                /* Clear ACK */
                i2c._regs->CR1 &= ~I2C_CR1_ACK;
                /* Program the STOP */
                i2c._regs->CR1 |= I2C_CR1_STOP;
            }
            SR1 = 0;
            SR2 = 0;
        }
    }
}

void I2Cx_dispatchError(I2Cx &i2c)
{
    if ((i2c._regs->SR1 & I2C_SR1_AF) == I2C_SR1_AF) {
        i2c._regs->SR1 &= ~I2C_SR1_AF; // <-- Clear BERR flag
        i2c._errors |= I2C_ERROR_AF;
    }

    if ((i2c._regs->SR1 & I2C_SR1_ARLO) == I2C_SR1_ARLO) {
        i2c._regs->SR1 &= ~I2C_SR1_ARLO; // <-- Clear ARLO flag
        i2c._errors |= I2C_ERROR_ARLO;
    }

    if ((i2c._regs->SR1 & I2C_SR1_BERR) == I2C_SR1_BERR) {
        i2c._regs->SR1 &= ~I2C_SR1_BERR;
        i2c._errors |= I2C_ERROR_BERR; // <-- Clear BERR flag
    }

    if ((i2c._regs->SR1 & I2C_SR1_OVR) == I2C_SR1_OVR) {
        i2c._regs->SR1 &= ~I2C_SR1_OVR; // <-- Clear OVR flag
        i2c._errors |= I2C_ERROR_OVR;
    }

    /* If any error - call handler, if set */
    if (i2c._errors != I2C_ERROR_NONE && i2c._onError) {
        i2c._onError(i2c, i2c._errors);
    }
}

