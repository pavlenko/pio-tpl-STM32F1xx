#ifndef __STM32_USB_H__
#define __STM32_USB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <usb/usbd.h>

//TODO optimize examples for own usage
// @see https://github.com/COKPOWEHEU/usb/blob/main/1.Core_F1/src/usb_lib.c

#define USB_EPx(num) (((volatile uint16_t*)USB)[(num)*2])

typedef struct usb_epdata_s {
    volatile uint32_t tx_addr;
    volatile union {
        uint32_t tx_count;
        struct {
            uint32_t tx_count_val:10;
            uint32_t tx_num_blocks:5;
            uint32_t tx_block_size:1;
        };
    };
    volatile uint32_t rx_addr;
    volatile union {
        uint32_t rx_count;
        struct {
            uint32_t rx_count_val:10;
            uint32_t rx_num_blocks:5;
            uint32_t rx_block_size:1;
        };
    };
} usb_epdata_t;

#define usb_epdata ((volatile usb_epdata_t*)(USB_PMAADDR))

extern usb_driver_t st_usb_driver;

#ifdef __cplusplus
}
#endif

#endif // __STM32_USB_H__
