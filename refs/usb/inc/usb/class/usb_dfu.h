#ifndef __USB_DFU_H__
#define __USB_DFU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#define DFU_DESCRIPTOR_TYPE_FUNCTIONAL 0x21

#define DFU_REQ_MASK (USB_REQ_TYPE_MASK | USB_REQ_RECIPIENT_MASK)
#define DFU_REQ_TYPE (USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE)

#define DFU_ATTR_CAN_DOWNLOAD      0x01
#define DFU_ATTR_CAN_UPLOAD        0x02
#define DFU_ATTR_MANIFEST_TOLERANT 0x04
#define DFU_ATTR_WILL_DETACH       0x08

#ifndef DFU_CAN_DETACH
#define DFU_CAN_DETACH true
#endif

#ifndef DFU_CAN_DNLOAD
#define DFU_CAN_DNLOAD true
#endif

#ifndef DFU_CAN_UPLOAD
#define DFU_CAN_UPLOAD false
#endif

typedef enum dfu_request_s {
    DFU_REQUEST_DETACH,
    DFU_REQUEST_DNLOAD,
    DFU_REQUEST_UPLOAD,
    DFU_REQUEST_GET_STATUS,
    DFU_REQUEST_CLR_STATUS,
    DFU_REQUEST_GET_STATE,
    DFU_REQUEST_ABORT,
} dfu_request_t;

typedef enum dfu_status_e {
    DFU_STATUS_OK,
    DFU_STATUS_ERR_TARGET,
    DFU_STATUS_ERR_FILE,
    DFU_STATUS_ERR_WRITE,
    DFU_STATUS_ERR_ERASE,
    DFU_STATUS_ERR_CHECK_ERASED,
    DFU_STATUS_ERR_PROG,
    DFU_STATUS_ERR_VERIFY,
    DFU_STATUS_ERR_ADDRESS,
    DFU_STATUS_ERR_NOTDONE,
    DFU_STATUS_ERR_FIRMWARE,
    DFU_STATUS_ERR_VENDOR,
    DFU_STATUS_ERR_USBR,
    DFU_STATUS_ERR_POR,
    DFU_STATUS_ERR_UNKNOWN,
    DFU_STATUS_ERR_STALLEDPKT,
} dfu_status_t;

typedef enum dfu_state_e {
    DFU_STATE_APP_IDLE,
    DFU_STATE_APP_DETACH,
    DFU_STATE_DFU_IDLE,
    DFU_STATE_DFU_DNLOAD_SYNC,
    DFU_STATE_DFU_DNBUSY,
    DFU_STATE_DFU_DNLOAD_IDLE,
    DFU_STATE_DFU_MANIFEST_SYNC,
    DFU_STATE_DFU_MANIFEST,
    DFU_STATE_DFU_MANIFEST_WAIT_RESET,
    DFU_STATE_DFU_UPLOAD_IDLE,
    DFU_STATE_DFU_ERROR,
} dfu_state_t;

typedef struct dfu_functional_descriptor_s {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bmAttributes;
    uint16_t wDetachTimeout;
    uint16_t wTransferSize;
    uint16_t bcdDFUVersion;
} __attribute__((packed)) dfu_functional_descriptor_t;

/* DFU context data */
typedef struct dfu_s {
    /* Current DFU status */
    dfu_status_t status;

    /* Current DFU state */
    dfu_state_t state;

    /* Download buffer, must be less than usb */
    uint8_t *dnload_buf;

    /* DFU download length (wLength) */
    uint16_t dnload_len;

    /* DNLOAD/UPLOAD block number (wValue) */
    uint16_t block_num;//<-- maybe rename

    //TODO avoid use offset, use block_num from protocol
    /* DNLOAD/UPLOAD operations offset */
    size_t offset;

    /** @deprecated */
    bool manifestation_complete;

    /**
     * User callback for reset app in dfu mode, here can store some vars which is reset independent
     */
    void (*cb_reset_to_dfu)(void);

    /**
     * User callback for reset dfu in app mode
     */
    void (*cb_reset_to_app)(void);

    /**
     * Copy received buffer to memory
     *
     * @param buf Buffer to copy from
     * @param len Length of data to copy
     * @param block_num Operation block number
     */
    void (*cb_buf_to_mem)(uint8_t *buf, size_t len, uint16_t block_num);
} dfu_t;

/* Include base header for allow use types above */
#include <usb/usbd.h>

usb_interface_t *dfu_setup(usb_interface_t *interface);

// upload callbacks
__attribute__((weak))
uint32_t dfu_get_upload_size();

__attribute__((weak))
uint8_t *dfu_get_upload_ptr(uint32_t offset);

#ifdef __cplusplus
}
#endif

#endif // __USB_DFU_H__
