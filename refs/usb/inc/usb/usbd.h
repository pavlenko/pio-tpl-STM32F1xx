#ifndef __USBD_H__
#define __USBD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <usb/usb_descriptors.h>

#ifndef __STATIC_FORCEINLINE
#define __STATIC_FORCEINLINE __attribute__((always_inline)) static inline
#endif

#ifndef USB_USE_CB_RESET
#define USB_USE_CB_RESET 1
#endif

#ifndef USB_USE_CB_SUSPEND
#define USB_USE_CB_SUSPEND 1
#endif

#ifndef USB_USE_CB_WAKEUP
#define USB_USE_CB_WAKEUP 1
#endif

#ifndef USB_USE_CB_SOF
#define USB_USE_CB_SOF 1
#endif

#ifndef USB_USE_CB_DOVR
#define USB_USE_CB_DOVR 1
#endif

#ifndef USB_USE_CB_ERROR
#define USB_USE_CB_ERROR 1
#endif

#ifndef USB_MAX_ENDPOINTS
#define USB_MAX_ENDPOINTS 8
#endif

#define USB_EP_DIR_MASK  0x80
#define USB_EP_ADDR_MASK 0x07

#define USB_EP_ADDR_IN(ep)  ((ep & USB_EP_ADDR_MASK) | USB_EP_DIR_MASK)
#define USB_EP_ADDR_OUT(ep) (ep & USB_EP_ADDR_MASK)

/* Operation result types */
typedef enum usb_result_e {
    USB_RESULT_NOTSUPP = 0,
    USB_RESULT_HANDLED = 1,
    USB_RESULT_NEXT_CALLBACK = 2,
    USB_RESULT_BUSY,
} usb_result_t;

/* USB endpoint 0 control states */
typedef enum usb_state_e {
    USB_EP0_STATE_IDLE,
    USB_EP0_STATE_STALLED,
    USB_EP0_STATE_DATA_IN,
    USB_EP0_STATE_LAST_DATA_IN,
    USB_EP0_STATE_DATA_OUT,
    USB_EP0_STATE_LAST_DATA_OUT,
    USB_EP0_STATE_STATUS_IN,
    USB_EP0_STATE_STATUS_OUT,
    USB_EP0_STATE_PAUSED,
} usb_state_t;

/* USB control handle */
typedef struct usb_control_s {
    usb_state_t state;
    usb_request_t req __attribute__((aligned(4)));
    uint8_t *ctrl_buf;
    uint16_t ctrl_len;
    bool needs_zlp;
} usb_control_t;

/* Pre definitions */
typedef struct usb_device_s usb_device_t;
typedef struct usb_interface_s usb_interface_t;
typedef union usb_class_ctx_u usb_class_ctx_t;

/* Class includes must be here */
#include <usb/class/usb_dfu.h>

/* Endpoint callback type */
typedef void (*usb_cb_endpoint_t)(usb_device_t *dev, uint8_t ep);

enum _usb_transaction {
    USB_TRANSACTION_IN,
    USB_TRANSACTION_OUT,
};

/* Interface specific context */
typedef union usb_class_ctx_u {
    void *ptr;
    dfu_t *dfu;
} usb_class_ctx_t;



/* Hardware specific driver */
typedef struct usb_driver_s {
    /**
     * Initialize HW
     */
    usb_device_t *(*init)(void);

    /**
     * Set an address
     *
     * @param dev  USB device handle structure
     * @param addr Device assigned address
     */
    void (*set_address)(usb_device_t *dev, uint8_t addr);

    /**
     * Setup an endpoint
     *
     * @param dev      USB device handle structure
     * @param ep       Full EP address including direction (e.g. 0x01 or 0x81)
     * @param type     Value for bmAttributes (USB_ENDPOINT_*)
     * @param max_size Endpoint max size
     * @param cb       Endpoint callback if needed
     */
	void (*ep_setup)(usb_device_t *dev, uint8_t ep, uint8_t type, uint16_t max_size, usb_cb_endpoint_t cb);

    /**
     * Reset all endpoints
     *
     * @param dev  USB device handle structure
     */
	void (*ep_reset)(usb_device_t *dev);

    /**
     * Get STALL status of an endpoint
     *
     * @param dev USB device handle structure
     * @param ep  Full EP address (with direction bit)
     * @return Non-zero if endpoint is stalled
     */
    bool (*ep_stall_get)(usb_device_t *dev, uint8_t ep);

    /**
     * Set/clr STALL condition on an endpoint
     *
     * @param dev   USB device handle structure
     * @param ep    Full EP address (with direction bit)
     * @param stall If 0, clear STALL, else set stall.
     */
	void (*ep_stall_set)(usb_device_t *dev, uint8_t ep, bool stall);

    /**
     * Set an Out endpoint to NAK
     *
     * @param dev USB device handle structure
     * @param ep  EP address
     * @param nak If non-zero, set NAK
     */
	void (*ep_nak_set)(usb_device_t *dev, uint8_t ep, bool nak);

    /**
     * Write a packet to endpoint
     *
     * @param dev USB device handle structure
     * @param ep  EP address (direction is ignored)
     * @param buf pointer to user data to write
     * @param len # of bytes
     * @return Actual # of bytes read
     */
    uint16_t (*ep_wr_packet)(usb_device_t *dev, uint8_t ep, const void *buf, uint16_t len);

    /**
     * Read a packet from endpoint
     *
     * @param dev USB device handle structure
     * @param ep  EP address (direction is ignored)
     * @param buf user buffer that will receive data
     * @param len # of bytes
     * @return Actual # of bytes read
     */
    uint16_t (*ep_rd_packet)(usb_device_t *dev, uint8_t ep, void *buf, uint16_t len);

    /**
     * Dispatch interrupts
     *
     * @param dev USB device handle structure
     */
    void (*poll)(usb_device_t *dev);

    /**
     * Disconnect device handler
     *
     * @param dev USB device handle structure
     * @param disconnected Disconnected flag
     */
    void (*disconnect)(usb_device_t *dev, bool disconnected);
} usb_driver_t;

/* USB interface definition */
typedef struct usb_interface_s {
    /* Interface specific context, pointer to any type */
    void *ctx;

    /**
     * STATUS_IN stage completed callback
     *
     * @param ctx Interface specific context
     * @param req USB request structure
     */
    void (*ep0_status_in)(void *ctx, usb_request_t *req);

    /**
     * STATUS_OUT stage completed callback
     *
     * @param ctx Interface specific context
     * @param req USB request structure
     */
    void (*ep0_status_out)(void *ctx, usb_request_t *req);

    /**
     * Non-Standard control request handler, called on each control setup request
     *
     * @param ctx Interface specific context
     * @param req USB request structure
     * @return Operation result status
     */
    usb_result_t (*ep0_setup_rd)(void *ctx, usb_request_t *req);

    /**
     * Non-Standard control request handler, called on each control setup request
     *
     * @param ctx Interface specific context
     * @param req USB request structure
     * @param buf user buffer that will receive data
     * @param len # of bytes
     * @return Operation result status
     */
    usb_result_t (*ep0_setup_wr)(void *ctx, usb_request_t *req, uint8_t **buf, uint16_t *len);

    /**
     * SET_CONFIGURATION request callback
     *
     * @param ctx Interface specific context
     * @param req USB request structure
     */
    void (*set_configuration)(void *ctx, usb_request_t *req);

    /**
     * SET_INTERFACE request callback
     *
     * @param ctx Interface specific context
     * @param req USB request structure
     */
    void (*set_interface)(void *ctx, usb_request_t *req);
} usb_interface_t;

/* USB Device handle */
typedef struct usb_device_s {
    /* Platform specific driver */
    const usb_driver_t *driver;

    /* Device descriptor pointer */
    const usb_device_descriptor_t *device_descr;

    /* Config descriptor(s) pointer */
    const usb_config_descriptor_t *config_descr;

    /* Strings array */
    const char * const *strings;

    /* Num strings */
    uint8_t num_strings;

    /* Current address */
    uint8_t curr_address;

    /* Current configuration (0 = unselected) */
    uint8_t curr_config;

    uint16_t pm_top;    /**< Top of allocated endpoint buffer memory */

    uint8_t *ctrl_buf;  /**< Internal buffer used for control transfers */
    uint16_t ctrl_len;

    usb_control_t control;

    usb_interface_t *interface;

    usb_cb_endpoint_t cb_endpoints[USB_MAX_ENDPOINTS][2];

    void (*cb_reset)(void);

    void (*cb_suspend)(void);

    void (*cb_wakeup)(void);

    void (*cb_sof)(void);

    void (*cb_dovr)(void);

    void (*cb_error)(void);
} usb_device_t;

void usbd_ep0_setup(usb_device_t *dev);
void usbd_ep0_in(usb_device_t *dev);
void usbd_ep0_out(usb_device_t *dev);

usb_device_t *usbd_setup(
    const usb_driver_t *driver,
    const usb_device_descriptor_t *device_descr,
    const usb_config_descriptor_t *config_descr,
    const char * const *strings,
    uint8_t num_strings,
    uint8_t *ctrl_buf,
    uint16_t ctrl_len
);

__STATIC_FORCEINLINE
void usbd_ep_setup(usb_device_t *dev, uint8_t ep, uint8_t type, uint16_t max_size, usb_cb_endpoint_t cb)
{
    dev->driver->ep_setup(dev, ep, type, max_size, cb);
}

__STATIC_FORCEINLINE
uint16_t usbd_ep_write_packet(usb_device_t *dev, uint8_t addr, const void *buf, uint16_t len)
{
    return dev->driver->ep_wr_packet(dev, addr, buf, len);
}

__STATIC_FORCEINLINE
uint16_t usbd_ep_read_packet(usb_device_t *dev, uint8_t addr, void *buf, uint16_t len)
{
    return dev->driver->ep_rd_packet(dev, addr, buf, len);
}

__STATIC_FORCEINLINE
void usbd_ep_stall_set(usb_device_t *dev, uint8_t addr, bool stall)
{
    dev->driver->ep_stall_set(dev, addr, stall);
}

__STATIC_FORCEINLINE
bool usbd_ep_stall_get(usb_device_t *dev, uint8_t addr)
{
    return dev->driver->ep_stall_get(dev, addr);
}

__STATIC_FORCEINLINE
void usbd_ep_nak_set(usb_device_t *dev, uint8_t addr, bool nak)
{
    dev->driver->ep_nak_set(dev, addr, nak);
}

void usbd_reset(usb_device_t *dev);

__STATIC_FORCEINLINE
void usbd_poll(usb_device_t *dev)
{
    dev->driver->poll(dev);
}

#ifdef __cplusplus
}
#endif

#endif // __USBD_H__