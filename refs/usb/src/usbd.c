#include <stdbool.h>
#include <stddef.h>
#include <usb/usbd.h>

#include "usb_private.h"

static void _usbd_ep0_data_in(usb_device_t *dev)
{
    if ((dev->control.ctrl_len == 0) && (dev->control.state == USB_EP0_STATE_LAST_DATA_IN)) {
        if (dev->control.needs_zlp) {
            /* No more data to send and empty packet */
            usbd_ep_write_packet(dev, 0, NULL, 0);
            dev->control.state = USB_EP0_STATE_LAST_DATA_IN;
            dev->control.needs_zlp = false;
        } else {
            /* No more data to send so STALL the TX Status*/
            dev->control.state = USB_EP0_STATE_STATUS_OUT;
            usbd_ep_stall_set(dev, 0, true);
        }
        return;
    }

    if (dev->device_descr->bMaxPacketSize0 < dev->control.ctrl_len) {
        /* Data stage, normal transmission */
        usbd_ep_write_packet(dev, 0, dev->control.ctrl_buf, dev->device_descr->bMaxPacketSize0);

        dev->control.state = USB_EP0_STATE_DATA_IN;
        dev->control.ctrl_buf += dev->device_descr->bMaxPacketSize0;
        dev->control.ctrl_len -= dev->device_descr->bMaxPacketSize0;
    } else {
        /* Data stage, end of transmission */
        usbd_ep_write_packet(dev, 0, dev->control.ctrl_buf, dev->control.ctrl_len);

        dev->control.state = dev->control.needs_zlp ? USB_EP0_STATE_DATA_IN : USB_EP0_STATE_LAST_DATA_IN;
        dev->control.needs_zlp = false;
        dev->control.ctrl_len = 0;
        dev->control.ctrl_buf = NULL;
    }
}

static void _usbd_ep0_data_out(usb_device_t *dev)
{
    if (dev->control.req.wLength) {
        uint16_t length = MIN(dev->device_descr->bMaxPacketSize0, dev->control.req.wLength - dev->control.ctrl_len);
        uint16_t readed = usbd_ep_read_packet(dev, 0, dev->control.ctrl_buf + dev->control.ctrl_len, length);

// if (size != packetsize) {
// 		stall_transaction(usbd_dev);
// 		return -1;
// 	}

        dev->control.ctrl_len += readed;
    }

    /* Wait for DATA OUT stage. */
    if (dev->control.req.wLength > dev->device_descr->bMaxPacketSize0) {
        dev->control.state = USB_EP0_STATE_DATA_OUT;
    } else if (dev->control.req.wLength > 0) {
        dev->control.state = USB_EP0_STATE_LAST_DATA_OUT;
    } else {
        dev->control.state = USB_EP0_STATE_STATUS_IN;
        dev->interface->ep0_status_in(dev->interface->ctx, &(dev->control.req));
        if (dev->control.state == USB_EP0_STATE_STALLED) {
            usbd_ep_stall_set(dev, 0, true);
        } else {
            usbd_ep_write_packet(dev, 0, NULL, 0);
        }
    }
}

static void _usbd_ep0_no_data(usb_device_t *dev)
{
    usb_request_t *req = &(dev->control.req);
    usb_result_t result = USB_RESULT_NOTSUPP;

    dev->control.ctrl_buf = dev->ctrl_buf;
    dev->control.ctrl_len = 0;

    if (req->bmRequestType == (USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_DEVICE)) {
        if (req->bRequest == USB_REQUEST_SET_CONFIGURATION) {
            result = usb_standard_set_configuration(dev, req);
        }
        else if (req->bRequest == USB_REQUEST_SET_ADDRESS) {
            result = usb_standard_set_address(dev, req);
        }
        else if (req->bRequest == USB_REQUEST_SET_FEATURE) {
            // Do nothing: result = USB_RESULT_NOTSUPP
        }
        else if (req->bRequest == USB_REQUEST_CLR_FEATURE) {
            // Do nothing: result = USB_RESULT_NOTSUPP
        }
    }
    else if (req->bmRequestType == (USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE)) {
        if (req->bRequest == USB_REQUEST_SET_INTERFACE) {
            result = usb_standard_set_interface(dev, req);
        }
    }
    else if (req->bmRequestType == (USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_ENDPOINT)) {
        if (req->bRequest == USB_REQUEST_SET_FEATURE) {
            result = usb_standard_set_feature(dev, req);
        }
        else if (req->bRequest == USB_REQUEST_CLR_FEATURE) {
            result = usb_standard_clr_feature(dev, req);
        }
    }

    if (result != USB_RESULT_HANDLED) {
        result = dev->interface->ep0_setup_rd(dev->interface->ctx, req);
        if (result == USB_RESULT_BUSY) {
            dev->control.state = USB_EP0_STATE_PAUSED;
            return;
        }
    }

    if (result == USB_RESULT_NOTSUPP) {
        dev->control.state = USB_EP0_STATE_STALLED;
    } else {
        dev->control.state = USB_EP0_STATE_STATUS_IN;
        usbd_ep_write_packet(dev, 0, NULL, 0);
    }
}

static void _usbd_ep0_wr_data(usb_device_t *dev)
{
    usb_request_t *req = &(dev->control.req);
    usb_result_t result = USB_RESULT_NOTSUPP;

    usb_result_t (*command)(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len) = NULL;

    dev->control.ctrl_buf = dev->ctrl_buf;
    dev->control.ctrl_len = 0;

    if (req->bRequest == USB_REQUEST_GET_DESCRIPTOR) {
        if (req->bmRequestType == (USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_DEVICE)) {
            uint8_t type = req->wValue >> 8;

            if (type == USB_DESCRIPTOR_TYPE_DEVICE) {
                command = usb_standard_get_descriptor_device;
            }
            if (type == USB_DESCRIPTOR_TYPE_CONFIG) {
                command = usb_standard_get_descriptor_config;
            }
            if (type == USB_DESCRIPTOR_TYPE_STRING) {
                command = usb_standard_get_descriptor_string;
            }
        }
    }
    else if (req->bRequest == USB_REQUEST_GET_STATUS) {
        //&& wLength == 2 && wIndex.1 == 0
        if (req->bmRequestType == (USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_DEVICE)) {
            //&& wIndex == 0
            command = usb_standard_get_status_device;
        }
        else if (req->bmRequestType == (USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE)) {
            //&& check support interface + altsetting supported
            //&& current config != 0
            command = usb_standard_get_status_interface;
        }
        else if (req->bmRequestType == (USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_ENDPOINT)) {
            //&& epnum without dir bit supported
            command = usb_standard_get_status_endpoint;
        }
    }
    else if (req->bRequest == USB_REQUEST_GET_CONFIGURATION) {
        if (req->bmRequestType == (USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_DEVICE)) {
            command = usb_standard_get_configuration;
        }
    }
    else if (req->bRequest == USB_REQUEST_GET_INTERFACE) {
        //&& current config != 0
        if (req->bmRequestType == (USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE)) {
            command = usb_standard_get_interface;
        }
    }

    if (command) {
        result = command(dev, req, &(dev->control.ctrl_buf), &(dev->control.ctrl_len));
    } else {
        result = dev->interface->ep0_setup_wr(dev->interface->ctx, req, &(dev->control.ctrl_buf), &(dev->control.ctrl_len));
        if (result == USB_RESULT_BUSY) {
            dev->control.state = USB_EP0_STATE_PAUSED;
            return;
        }
    }

    if (dev->control.ctrl_len == 0xFFFF) {
        dev->control.state = USB_EP0_STATE_PAUSED;
    } else if (result == USB_RESULT_NOTSUPP || dev->control.ctrl_len == 0) {
        dev->control.state = USB_EP0_STATE_STALLED;
        return;
    }

    if (req->bmRequestType & USB_REQ_DIRECTION_IN) {
        dev->control.needs_zlp = false;
        if (dev->control.ctrl_len < req->wLength) {
            if (dev->control.ctrl_len && (dev->control.ctrl_len % dev->device_descr->bMaxPacketSize0 == 0)) {
                dev->control.needs_zlp = true;
            }
        }

        _usbd_ep0_data_in(dev);
    } else {
        dev->control.state = USB_EP0_STATE_DATA_OUT;
        usbd_ep_nak_set(dev, 0, false);
    }
}

static void _usbd_ep0_complete(usb_device_t *dev)
{
    if (dev->control.state == USB_EP0_STATE_STALLED) {
        usbd_ep_stall_set(dev, 0, true);
    }
}

void usbd_ep0_setup(usb_device_t *dev)
{
    usb_request_t *req = &(dev->control.req);

    usbd_ep_nak_set(dev, 0, true);

    if (dev->driver->ep_rd_packet(dev, 0, req, 8) != 8) {
        usbd_ep_stall_set(dev, 0, true);
        dev->control.state = USB_EP0_STATE_IDLE;
        return;
    }

    if (req->wLength == 0) {
        _usbd_ep0_no_data(dev);
    } else {
        _usbd_ep0_wr_data(dev);
    }

    _usbd_ep0_complete(dev);
}

void usbd_ep0_in(usb_device_t *dev)
{
    usb_request_t *req = &(dev->control.req);

    if (dev->control.state == USB_EP0_STATE_DATA_IN || dev->control.state == USB_EP0_STATE_LAST_DATA_IN) {
        _usbd_ep0_data_in(dev);
    } else if (dev->control.state == USB_EP0_STATE_STATUS_IN) {
        /* Exception: Handle SET ADDRESS function here... */
        if (req->bmRequestType == 0 && req->bRequest == USB_REQUEST_SET_ADDRESS) {
            dev->driver->set_address(dev, req->wValue);
        }
        dev->interface->ep0_status_in(dev->interface->ctx, req);
        dev->control.state = USB_EP0_STATE_IDLE;
        //TODO maybe IDLE like in libopencm3
    } else {
        dev->control.state = USB_EP0_STATE_STALLED;
    }

    return _usbd_ep0_complete(dev);
}

void usbd_ep0_out(usb_device_t *dev)
{
    if (dev->control.state == USB_EP0_STATE_DATA_IN || dev->control.state == USB_EP0_STATE_LAST_DATA_IN) {
        dev->control.state = USB_EP0_STATE_STALLED;
    } else if (dev->control.state == USB_EP0_STATE_DATA_OUT || dev->control.state == USB_EP0_STATE_LAST_DATA_OUT) {
        _usbd_ep0_data_out(dev);
    } else if (dev->control.state == USB_EP0_STATE_STATUS_OUT) {
        dev->interface->ep0_status_out(dev->interface->ctx, &(dev->control.req));
        dev->control.state = USB_EP0_STATE_IDLE;
        //TODO maybe IDLE like in libopencm3
    } else {
        dev->control.state = USB_EP0_STATE_STALLED;
    }

    _usbd_ep0_complete(dev);
}
//TODO check ep0 callbacks code!!!!!!!!!!!

usb_device_t *usbd_setup(
    const usb_driver_t *driver,
    const usb_device_descriptor_t *device_descr,
    const usb_config_descriptor_t *config_descr,
    const char * const *strings,
    uint8_t num_strings,
    uint8_t *ctrl_buf,
    uint16_t ctrl_len
) {
    usb_device_t *dev = driver->init();

    dev->driver = driver;
    dev->device_descr = device_descr;
    dev->config_descr = config_descr;
    dev->strings = strings;
    dev->num_strings = num_strings;
    dev->ctrl_buf = ctrl_buf;
    dev->ctrl_len = ctrl_len;

    return dev;
}

void usbd_reset(usb_device_t *dev)
{
    dev->curr_address = 0;
    dev->curr_config = 0;

    usbd_ep_setup(dev, 0, USB_ENDPOINT_TRANSFER_TYPE_CONTROL, dev->device_descr->bMaxPacketSize0, NULL);

    dev->driver->set_address(dev, 0);

    if (dev->cb_reset) {
        dev->cb_reset();
    }
}