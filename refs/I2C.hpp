#ifndef __STM32_I2C_HPP__
#define __STM32_I2C_HPP__

#include <functional>
#include <stdint.h>

#include <stm32f1xx.h>

#ifndef I2C_IRQ_PRIORITY
#define I2C_IRQ_PRIORITY 2
#endif
#ifndef I2C_IRQ_SUBPRIORITY
#define I2C_IRQ_SUBPRIORITY 0
#endif

typedef enum I2C_mode_s : uint8_t {
    I2C_MODE_NONE   = 0x00U,   /*!< No I2C communication on going             */
    I2C_MODE_MASTER = 0x10U,   /*!< I2C communication is in Master Mode       */
    I2C_MODE_SLAVE  = 0x20U,   /*!< I2C communication is in Slave Mode        */
    I2C_MODE_MEM    = 0x40U,   /*!< I2C communication is in Memory Mode       */
} I2C_mode_t;

typedef enum I2C_speed_s : uint32_t {
    I2C_SPEED_STANDARD  = 100000,
    I2C_SPEED_FAST      = 400000,
    I2C_SPEED_FAST_PLUS = 1000000,
} I2C_speed_t;

typedef enum : uint8_t {
    I2C_STATE_RESET          = 0x00U, // Peripheral is not yet Initialized
    I2C_STATE_READY          = 0x20U, // Peripheral Initialized and ready for use
    I2C_STATE_BUSY           = 0x24U, // An internal process is ongoing
    I2C_STATE_BUSY_TX        = 0x21U, // Data Transmission process is ongoing
    I2C_STATE_BUSY_RX        = 0x22U, // Data Reception process is ongoing
    I2C_STATE_LISTEN         = 0x28U, // Address Listen Mode is ongoing
    I2C_STATE_BUSY_TX_LISTEN = 0x29U, // Address Listen Mode and Data Transmission process is ongoing
    I2C_STATE_BUSY_RX_LISTEN = 0x2AU, // Address Listen Mode and Data Reception process is ongoing
    I2C_STATE_ABORT          = 0x60U, // Abort user request ongoing
    I2C_STATE_TIMEOUT        = 0xA0U, // Timeout state
    I2C_STATE_ERROR          = 0xE0U, // Error
} I2C_state_t;

#define I2C_ERROR_NONE      0x0000U    /*!< No error              */
#define I2C_ERROR_BERR      0x0001U    /*!< BERR error            */
#define I2C_ERROR_ARLO      0x0002U    /*!< ARLO error            */
#define I2C_ERROR_AF        0x0004U    /*!< AF error              */
#define I2C_ERROR_OVR       0x0008U    /*!< OVR error             */
#define I2C_ERROR_DMA       0x0010U    /*!< DMA transfer error    */
#define I2C_ERROR_TIMEOUT   0x0020U    /*!< Timeout Error         */
#define I2C_ERROR_SIZE      0x0040U    /*!< Size Management error */
#define I2C_ERROR_DMA_PARAM 0x0080U    /*!< DMA Parameter Error   */
#define I2C_WRONG_START     0x0200U    /*!< Wrong start Error     */

class I2Cx
{
    friend void I2Cx_dispatchEvent(I2Cx &i2c);
    friend void I2Cx_dispatchError(I2Cx &i2c);
public:
    typedef std::function<void(I2Cx &i2c, bool isTransmit)> cbAddress_t;
    typedef std::function<void(uint8_t *data, size_t size)> cbReceive_t;
    typedef std::function<void(I2Cx &drv)> cbRequest_t;
    typedef std::function<void(I2Cx &i2c, uint8_t errors)> cbError_t;
private:
    I2C_TypeDef *_regs;
    __IO I2C_mode_t _mode;
    __IO I2C_state_t _state;
    uint8_t _ownAddress;
    uint8_t _devAddress;
    uint8_t *_dataBuf;
    uint16_t _dataLen;
    __IO uint16_t _dataCnt;
    __IO uint16_t _errors;
    cbAddress_t _onAddress;
    cbReceive_t _onReceive;
    cbRequest_t _onRequest;
    cbError_t _onError;
public:
    /**
     * @brief Construct a new I2Cx object
     *
     * @param regs Pointer to I2C registers structure
     */
    I2Cx(I2C_TypeDef *regs): _regs(regs)
    {};

    /**
     * @brief Initialize as master
     *
     * @param speed I2C standard speed
     */
    void init(I2C_speed_t speed);

    /**
     * @brief Initialize as slave
     *
     * @param speed       I2C standard speed
     * @param ownAddress  Address for listen to
     * @param generalCall Allow handle general call address
     * @param noStrech    Enable no-strech for
     */
    void init(I2C_speed_t speed, uint8_t ownAddress, bool generalCall, bool noStrech);

    void masterSend_IT(uint16_t devAddress, uint8_t *buf, uint16_t len);
    void masterRecv_IT(uint16_t devAddress, uint8_t *buf, uint16_t len);

    void slaveSend_IT(uint8_t *buf, uint16_t len);
    void slaveRecv_IT(uint8_t *buf, uint16_t len);

    // void memSet_IT(uint16_t devAddress, uint8_t memAddress, uint8_t *buf, uint16_t len);
    // void memSet_IT(uint16_t devAddress, uint16_t memAddress, uint8_t *buf, uint16_t len);
    // void memGet_IT(uint16_t devAddress, uint8_t memAddress, uint8_t *buf, uint16_t len);
    // void memGet_IT(uint16_t devAddress, uint16_t memAddress, uint8_t *buf, uint16_t len);

    void onAddress(cbAddress_t cb);
    void onReceive(cbReceive_t cb);
    void onRequest(cbRequest_t cb);
    void onError(cbError_t cb);
};

#endif // __STM32_I2C_HPP__