#ifndef __USB_COMPOSITE_H__
#define __USB_COMPOSITE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <usb/usbd.h>

typedef struct usb_composite_ctx_s {
    usb_interface_t *arr_interfaces;
    uint8_t num_interfaces;
} usb_composite_ctx_t;

void usb_composite_setup(
    usb_interface_t *interface,
    usb_composite_ctx_t *ctx,
    usb_interface_t *arr_interfaces,
    uint8_t num_interfaces
);

#ifdef __cplusplus
}
#endif

#endif // __USB_COMPOSITE_H__