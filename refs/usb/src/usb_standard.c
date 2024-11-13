#include <string.h>
#include "usb_private.h"

usb_result_t usb_standard_get_status_device(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    (void) dev;
    (void) req;

    if (*len > 2) {
        *len = 2;
    }

    (*buf)[0] = 0;// self powered
    (*buf)[1] = 0;// remote wakeup

    return USB_RESULT_HANDLED;
}

usb_result_t usb_standard_get_status_interface(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    (void) dev;
    (void) req;
    /* not defined */
    if (*len > 2) {
        *len = 2;
    }
    (*buf)[0] = 0;
    (*buf)[1] = 0;

    return USB_RESULT_HANDLED;
}

usb_result_t usb_standard_get_status_endpoint(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    if (*len > 2) {
        *len = 2;
    }
    (*buf)[0] = dev->driver->ep_stall_get(dev, req->wIndex) ? 1 : 0;
    (*buf)[1] = 0;

    return USB_RESULT_HANDLED;
}

usb_result_t usb_standard_set_address(usb_device_t *dev, usb_request_t *req)
{
    /* The actual address is only latched at the STATUS IN stage. */
    if ((req->bmRequestType != 0) || (req->wValue >= 128)) {
        return USB_RESULT_NOTSUPP;
    }

    dev->curr_address = req->wValue;
    return USB_RESULT_HANDLED;
}

usb_result_t usb_standard_get_descriptor_device(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    (void) req;
    *buf = (uint8_t *) dev->device_descr;
    *len = MIN(*len, dev->device_descr->bLength);
    return USB_RESULT_HANDLED;
}

usb_result_t usb_standard_get_descriptor_config(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    uint8_t *ptr = *buf;
    uint8_t i, j, k;
    uint16_t count, total = 0, wTotalLength = 0;

    *buf = dev->ctrl_buf;

    const usb_config_descriptor_t *config = &dev->config_descr[req->wValue & 0xFF];
    count = (uint16_t) MIN(*len, config->bLength);

    memcpy(*buf, config, count);
    *buf += count;
    *len -= count;
    total += count;
    wTotalLength += config->bLength;

    /* For each interface... */
    for (i = 0; i < config->bNumInterfaces; i++) {
        /* Interface Association Descriptor, if any */
        if (config->interfaces[i].p_association) {
            const usb_interface_assoc_descriptor_t *assoc = config->interfaces[i].p_association;
            count = (uint16_t) MIN(*len, assoc->bLength);

            memcpy(*buf, assoc, count);
            *buf += count;
            *len -= count;
            total += count;
            wTotalLength += assoc->bLength;
        }

        /* For each alternate setting... */
        for (j = 0; j < config->interfaces[i].n_altsettings; j++) {
            /* Copy interface descriptor. */
            const usb_interface_descriptor_t *interface = &config->interfaces[i].a_altsettings[j];
            count = (uint16_t) MIN(*len, interface->bLength);

            memcpy(*buf, interface, count);
            *buf += count;
            *len -= count;
            total += count;
            wTotalLength += interface->bLength;

            /* Copy extra bytes (function descriptors), if any . */
            if (interface->extra_ptr) {
                count = (uint16_t) MIN(*len, interface->extra_len);

                memcpy(*buf, interface->extra_ptr, count);
                *buf += count;
                *len -= count;
                total += count;
                wTotalLength += interface->extra_len;
            }

            /* For each endpoint... */
            for (k = 0; k < interface->bNumEndpoints; k++) {
                /* Copy endpoint descriptor */
                const usb_endpoint_descriptor_t *endpoint = &interface->endpoints[k];
                count = (uint16_t) MIN(*len, endpoint->bLength);

                memcpy(*buf, endpoint, count);
                *buf += count;
                *len -= count;
                total += count;
                wTotalLength += endpoint->bLength;

                /* Copy extra bytes (class specific). */
                if (endpoint->extra_ptr) {
                    count = (uint16_t) MIN(*len, endpoint->extra_len);

                    memcpy(*buf, endpoint->extra_ptr, count);
                    *buf += count;
                    *len -= count;
                    total += count;
                    wTotalLength += endpoint->extra_len;
                }
            }
        }
    }

    memcpy(ptr + 2, &wTotalLength, sizeof(uint16_t));
    *len = total;

    return USB_RESULT_HANDLED;
}

usb_result_t usb_standard_get_descriptor_string(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    uint16_t i, array_idx, descr_idx = (uint16_t)(req->wValue & 0xFF);
    usb_string_descriptor_t *sd = (usb_string_descriptor_t *) dev->ctrl_buf;

    if (descr_idx == 0) {
        /* Send sane Language ID descriptor... */
        sd->wData[0] = USB_LANGID_ENGLISH_US;
        sd->bLength = sizeof(sd->bLength) + sizeof(sd->bDescriptorType) + sizeof(sd->wData[0]);

        *len = MIN(*len, sd->bLength);
    } else {
        array_idx = descr_idx - 1;

        if (!dev->strings) {
            return USB_RESULT_NOTSUPP;// Device doesn't support strings.
        }

        if (array_idx >= dev->num_strings) {
            return USB_RESULT_NOTSUPP;// String index is not in range.
        }

        /* Strings with Language ID differnet from USB_LANGID_ENGLISH_US are not supported */
        if (req->wIndex != USB_LANGID_ENGLISH_US) {
            return USB_RESULT_NOTSUPP;
        }

        /* This string is returned as UTF16, hence the multiplication */
        sd->bLength = (strlen(dev->strings[array_idx]) * 2) + sizeof(sd->bLength) + sizeof(sd->bDescriptorType);

        *len = MIN(*len, sd->bLength);

        for (i = 0; i < (*len / 2) - 1; i++) {
            sd->wData[i] = dev->strings[array_idx][i];
        }
    }

    sd->bDescriptorType = USB_DESCRIPTOR_TYPE_STRING;
    *buf = (uint8_t *) sd;

    return USB_RESULT_HANDLED;
}

usb_result_t usb_standard_get_configuration(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    (void) req;

    if (*len > 1) {
        *len = 1;
    }

    if (dev->curr_config > 0) {
        (*buf)[0] = dev->config_descr[dev->curr_config - 1].bConfigurationValue;
    } else {
        (*buf)[0] = 0;
    }

    return USB_RESULT_HANDLED;
}

usb_result_t usb_standard_set_configuration(usb_device_t *dev, usb_request_t *req)
{
    uint8_t i;
    int8_t found_index = -1;
    const usb_config_descriptor_t *config;

    if (req->wValue > 0) {
        for (i = 0; i < dev->device_descr->bNumConfigurations; i++) {
            if (req->wValue == dev->config_descr[i].bConfigurationValue) {
                found_index = i;
                break;
            }
        }
        if (found_index < 0) {
            return USB_RESULT_NOTSUPP;
        }
    }

    dev->curr_config = found_index + 1;

    if (dev->curr_config > 0) {
        config = &dev->config_descr[dev->curr_config - 1];

        /* reset all alternate settings configuration */
        for (i = 0; i < config->bNumInterfaces; i++) {
            config->interfaces[i].i_altsetting = 0;
        }
    }

    dev->driver->ep_reset(dev);

    dev->interface->set_configuration(dev->interface->ctx, req);

    return USB_RESULT_HANDLED;
}

usb_result_t usb_standard_get_interface(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    const usb_config_descriptor_t *config = &dev->config_descr[dev->curr_config - 1];

    if (req->wIndex >= config->bNumInterfaces) {
        return USB_RESULT_NOTSUPP;
    }

    *len = 1;
    (*buf)[0] = config->interfaces[req->wIndex].i_altsetting;

    return USB_RESULT_HANDLED;
}

usb_result_t usb_standard_set_interface(usb_device_t *dev, usb_request_t *req)
{
    const usb_config_descriptor_t *config = &dev->config_descr[dev->curr_config - 1];
    usb_config_interface_t *interface;

    if (req->wIndex >= config->bNumInterfaces) {
        return USB_RESULT_NOTSUPP;
    }

    interface = &config->interfaces[req->wIndex];

    if (req->wValue >= interface->n_altsettings) {
        return USB_RESULT_NOTSUPP;
    }

    if (interface->i_altsetting != req->wValue) {
        interface->i_altsetting = req->wValue;
    } else if (req->wValue > 0) {
        return USB_RESULT_NOTSUPP;
    }

    dev->interface->set_interface(dev->interface->ctx, req);

    return USB_RESULT_HANDLED;
}

usb_result_t usb_standard_set_feature(usb_device_t *dev, usb_request_t *req)
{
    if (req->bmRequestType == (USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_ENDPOINT)) {
        dev->driver->ep_stall_set(dev, req->wIndex, 1);
    }
    return USB_RESULT_HANDLED;
}

usb_result_t usb_standard_clr_feature(usb_device_t *dev, usb_request_t *req)
{
    if (req->bmRequestType == (USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_ENDPOINT)) {
        dev->driver->ep_stall_set(dev, req->wIndex, 0);
    }
    return USB_RESULT_HANDLED;
}
