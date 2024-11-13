#include <usb/class/usb_composite.h>

#include <stdlib.h>

//TODO composite functions

void usb_composite_setup(
    usb_interface_t *interface,
    usb_composite_ctx_t *ctx,
    usb_interface_t *arr_interfaces,
    uint8_t num_interfaces
) {
    ctx->arr_interfaces = arr_interfaces;
    ctx->num_interfaces = num_interfaces;

    //interface->ctx = ctx;

   // interface->ep0_setup_rd = _usb_composite_ep0_setup_rd;
    //interface->ep0_setup_wr = _usb_composite_ep0_setup_wr;
}