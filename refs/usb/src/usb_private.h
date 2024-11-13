#ifndef __USB_PRIVATE_H__
#define __USB_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <usb/usbd.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))


//TODO std apis
usb_result_t usb_standard_get_status_device(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
usb_result_t usb_standard_get_status_interface(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
usb_result_t usb_standard_get_status_endpoint(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
usb_result_t usb_standard_set_address(usb_device_t *dev, usb_request_t *req);
usb_result_t usb_standard_get_descriptor_device(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
usb_result_t usb_standard_get_descriptor_config(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
usb_result_t usb_standard_get_descriptor_string(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
usb_result_t usb_standard_get_configuration(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
usb_result_t usb_standard_set_configuration(usb_device_t *dev, usb_request_t *req);
usb_result_t usb_standard_get_interface(usb_device_t *dev, usb_request_t *req, uint8_t **buf, uint16_t *len);
usb_result_t usb_standard_set_interface(usb_device_t *dev, usb_request_t *req);
usb_result_t usb_standard_set_feature(usb_device_t *dev, usb_request_t *req);
usb_result_t usb_standard_clr_feature(usb_device_t *dev, usb_request_t *req);

#ifdef __cplusplus
}
#endif

#endif // __USB_PRIVATE_H__