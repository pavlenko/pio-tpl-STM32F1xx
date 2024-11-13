#include <stdbool.h>
#include <stm32f1xx.h>
#include <stm32/_common.h>
#include <stm32/rcc.h>
#include <stm32/usb.h>
#include <usb/usbd.h>

#define USBD_PM_TOP 0x40

#define MIN(a, b) ((a) < (b) ? (a) : (b))

/* USB control related macro */
#define USB_CLR_ISTR_PMAOVR() (USB->ISTR &= ~USB_ISTR_PMAOVR)
#define USB_CLR_ISTR_ERR()    (USB->ISTR &= ~USB_ISTR_ERR)
#define USB_CLR_ISTR_WKUP()   (USB->ISTR &= ~USB_ISTR_WKUP)
#define USB_CLR_ISTR_SUSP()   (USB->ISTR &= ~USB_ISTR_SUSP)
#define USB_CLR_ISTR_RESET()  (USB->ISTR &= ~USB_ISTR_RESET)
#define USB_CLR_ISTR_SOF()    (USB->ISTR &= ~USB_ISTR_SOF)
#define USB_CLR_ISTR_ESOF()   (USB->ISTR &= ~USB_ISTR_ESOF)

#define USB_EPnR(ep) ((uint16_t *)(USB_EP0R + ep))

#define EP0REG ((__IO unsigned *)(USB_BASE))
#define USB_EPnR_GET(ep) ((uint16_t)(*(EP0REG + ep)))
#define USB_EPnR_SET(ep, val) (*(EP0REG + ep) = (uint16_t)val)

#define USB_GET_EP_TYPE(ep) (USB_EPnR_GET(ep) & USB_EP_T_FIELD)
#define USB_SET_EP_TYPE(ep,type) (USB_EPnR_SET(ep, ((USB_EPnR_GET(ep) & USB_EP_T_MASK) | type )))

#define USB_CLR_EP_RX_CTR(ep) (USB_EPnR_SET(ep, USB_EPnR_GET(ep) & ~USB_EP_CTR_RX & USB_EPREG_MASK))
#define USB_CLR_EP_TX_CTR(ep) (USB_EPnR_SET(ep, USB_EPnR_GET(ep) & ~USB_EP_CTR_TX & USB_EPREG_MASK))

#define USB_TOG_EP_RX_DTOG(ep) (USB_EPnR_SET(ep, USB_EP_CTR_RX | USB_EP_CTR_TX | USB_EP_DTOG_RX | (USB_EPnR_GET(ep) & USB_EPREG_MASK)))
#define USB_TOG_EP_TX_DTOG(ep) (USB_EPnR_SET(ep, USB_EP_CTR_RX | USB_EP_CTR_TX | USB_EP_DTOG_TX | (USB_EPnR_GET(ep) & USB_EPREG_MASK)))

#define USB_CLR_EP_RX_DTOG(ep) if (USB_EPnR_GET(ep) & USB_EP_DTOG_RX) USB_TOG_EP_RX_DTOG(ep)
#define USB_CLR_EP_TX_DTOG(ep) if (USB_EPnR_GET(ep) & USB_EP_DTOG_TX) USB_TOG_EP_TX_DTOG(ep)

//TODO upd stat logic, how?????
#define USB_GET_EP_RX_STAT(ep) ((uint16_t) USB_EPnR_GET(ep) & USB_EPRX_STAT)
#define USB_SET_EP_RX_STAT(ep, stat) (*USB_EPnR(ep) = (*USB_EPnR(ep) & USB_EPREG_MASK & ~USB_EPRX_STAT) | stat)


#define USB_GET_EP_TX_STAT(ep) ((uint16_t) USB_EPnR_GET(ep) & USB_EPTX_STAT)
#define USB_SET_EP_TX_STAT(ep, stat) (*USB_EPnR(ep) = (*USB_EPnR(ep) & USB_EPREG_MASK & ~USB_EPTX_STAT) | stat)
//TODO end todo...

#define USB_GET_EP_ADDR(ep) (USB_EPnR_GET(ep) & USB_EPADDR_FIELD)
#define USB_SET_EP_ADDR(ep, addr) (USB_EPnR_SET(ep, USB_EP_CTR_RX | USB_EP_CTR_TX | (USB_EPnR_GET(ep) & USB_EPREG_MASK) | addr))

//TODO see st fs lib (no hal)

/* PMA related macro */
#define USB_ADDRn_TX(ep) ((uint32_t *)((USB->BTABLE + (ep) * 8 + 0) * 2 + USB_PMAADDR))
#define USB_COUNTn_TX(ep) ((uint32_t *)((USB->BTABLE + (ep) * 8 + 2) * 2 + USB_PMAADDR))
#define USB_ADDRn_RX(ep) ((uint32_t *)((USB->BTABLE + (ep) * 8 + 4) * 2 + USB_PMAADDR))
#define USB_COUNTn_RX(ep) ((uint32_t *)((USB->BTABLE + (ep) * 8 + 6) * 2 + USB_PMAADDR))

#define USB_GET_EP_TX_ADDR(ep) ((uint16_t)*USB_ADDRn_TX(ep))
#define USB_GET_EP_RX_ADDR(ep) ((uint16_t)*USB_ADDRn_RX(ep))

#define USB_SET_EP_TX_ADDR(ep, addr) (*USB_ADDRn_TX(ep) = ((addr >> 1) << 1))
#define USB_SET_EP_RX_ADDR(ep, addr) (*USB_ADDRn_RX(ep) = ((addr >> 1) << 1))

#define USB_GET_EP_TX_COUNT(ep) ((uint16_t)(*USB_COUNTn_TX(ep)) & 0x3FF)
#define USB_GET_EP_RX_COUNT(ep) ((uint16_t)(*USB_COUNTn_RX(ep)) & 0x3FF)

#define USB_SET_EP_TX_COUNT(ep, count) (*USB_COUNTn_TX(ep) = count)
#define USB_SET_EP_RX_COUNT(ep, count) (*USB_COUNTn_RX(ep) = count)

#ifndef USB_ISTR_DOVR
#define USB_ISTR_DOVR (1U << 14)
#endif

uint8_t _force_nak[8];
usb_device_t _dev;

static void st_usb_pma_to_buf(uint8_t *buf, uint16_t addr, uint16_t len)
{
    uint16_t value;
    uint8_t *p_buf = buf;
    uint8_t odd = len & 1;
    len >>= 1;

    __IO uint16_t *pma = (__IO uint16_t *)(((uint32_t)addr * 2) + USB_PMAADDR);

    if (((uintptr_t) buf) & 0x01) {
        for (; len; pma++, len--) {
            value = *pma;
            *p_buf++ = value;
            *p_buf++ = value >> 8;
        }
    } else {
        for (; len; pma++, p_buf += 2, len--) {
            *(uint16_t *) p_buf = *pma;
        }
    }

    if (odd) {
        *p_buf = *(uint8_t *) pma;
    }
}

static void st_usb_buf_to_pma(const void *buf, uint16_t addr, uint16_t len)
{
    uint32_t i;
    const uint8_t *p_buf = buf;

    __IO uint16_t *pma = (__IO uint16_t *)(((uint32_t)addr * 2) + USB_PMAADDR);

    for (i = 0; i < len; i += 2) {
        *pma++ = (uint16_t)p_buf[i + 1] << 8 | p_buf[i];
    }
}

/**
 * Set the receive buffer size for a given USB endpoint.
 *
 * @param dev the usb device handle returned from @ref usbd_init
 * @param ep Index of endpoint to configure.
 * @param size Size in bytes of the RX buffer. Legal sizes : {2,4,6...62}; {64,96,128...992}.
 * @returns (uint16) Actual size set
 */
uint16_t st_usb_set_ep_rx_bufsize(usb_device_t *dev, uint8_t ep, uint32_t size)
{
    uint16_t realsize;
    (void)dev;
    /*
     * Writes USB_COUNTn_RX reg fields : bits <14:10> are NUM_BLOCK; bit 15 is BL_SIZE
     * - When (size <= 62), BL_SIZE is set to 0 and NUM_BLOCK set to (size / 2).
     * - When (size > 62), BL_SIZE is set to 1 and NUM_BLOCK=((size / 32) - 1).
     *
     * This algo rounds to the next largest legal buffer size, except 0. Examples:
     *	size =>	BL_SIZE, NUM_BLOCK	=> Actual bufsize
     *	0		0		0			??? "Not allowed" according to RM0091, RM0008
     *	1		0		1			2
     *	61		0		31			62
     *	63		1		1			64
     */
    if (size > 62) {
        /* Round up, div by 32 and sub 1 == (size + 31)/32 - 1 == (size-1)/32)*/
        size = ((size - 1) >> 5) & 0x1F;
        realsize = (size + 1) << 5;
        /* Set BL_SIZE bit (no macro for this) */
        size |= (1<<5);
    } else {
        /* round up and div by 2 */
        size = (size + 1) >> 1;
        realsize = size << 1;
    }
    /* write to the BL_SIZE and NUM_BLOCK fields */
    USB_SET_EP_RX_COUNT(ep, size << 10);
    return realsize;
}

static usb_device_t *st_usb_init(void)
{
    rcc_periph_clk_enable(RCC_USB);

    USB->CNTR = 0;
    USB->BTABLE = 0;
    USB->ISTR = 0;

    /* Enable RESET, SUSPEND, RESUME and CTR interrupts. */
    USB->CNTR = (USB_CNTR_RESETM | USB_CNTR_CTRM | USB_CNTR_SUSPM | USB_CNTR_WKUPM | USB_CNTR_ERRM);

    return &_dev;
}

static void st_usb_set_address(usb_device_t *dev, uint8_t addr)
{
    (void)dev;
    /* Set device address and enable. */
    USB->DADDR = (addr & USB_DADDR_ADD) | USB_DADDR_EF;
}

void st_usb_ep_setup(usb_device_t *dev, uint8_t ep, uint8_t type, uint16_t max_size, usb_cb_endpoint_t cb)
{
    /* Translate USB standard type codes to STM32. */
    const uint16_t typelookup[] = {
        [USB_ENDPOINT_TRANSFER_TYPE_CONTROL] = USB_EP_CONTROL,
        [USB_ENDPOINT_TRANSFER_TYPE_ISOCHRONOUS] = USB_EP_ISOCHRONOUS,
        [USB_ENDPOINT_TRANSFER_TYPE_BULK] = USB_EP_BULK,
        [USB_ENDPOINT_TRANSFER_TYPE_INTERRUPT] = USB_EP_INTERRUPT,
    };

    uint8_t dir = ep & 0x80;
    ep &= 0x7F;

    /* Assign address. */
    USB_SET_EP_ADDR(ep, ep);
    USB_SET_EP_TYPE(ep, typelookup[type]);

    if (ep == 0) {
        USB_SET_EP_TX_ADDR(ep, dev->pm_top);
        USB_CLR_EP_TX_DTOG(ep);
        USB_SET_EP_TX_STAT(ep, USB_EP_TX_NAK);
        dev->pm_top += max_size;
        return;
    }

    if (dir) {
        USB_SET_EP_TX_ADDR(ep, dev->pm_top);
        if (cb) {
            dev->cb_endpoints[ep - 1][USB_TRANSACTION_IN] = cb;
        }
        USB_CLR_EP_TX_DTOG(ep);
        USB_SET_EP_TX_STAT(ep, USB_EP_TX_NAK);
        dev->pm_top += max_size;
    }

    if (!dir) {
        uint16_t realsize;
        USB_SET_EP_RX_ADDR(ep, dev->pm_top);
        if (cb) {
            dev->cb_endpoints[ep - 1][USB_TRANSACTION_OUT] = cb;
        }
        realsize = st_usb_set_ep_rx_bufsize(dev, ep, max_size);
        USB_CLR_EP_RX_DTOG(ep);
        USB_SET_EP_RX_STAT(ep, USB_EP_RX_VALID);
        dev->pm_top += realsize;
    }
}

static void st_usb_ep_reset(usb_device_t *dev)
{
    uint8_t i;

    /* Reset all endpoints. */
    for (i = 1; i < 8; i++) {
        USB_SET_EP_TX_STAT(i, USB_EP_TX_DIS);
        USB_SET_EP_RX_STAT(i, USB_EP_RX_DIS);
    }
    dev->pm_top = USBD_PM_TOP + (2 * dev->device_descr->bMaxPacketSize0);
}

static bool st_usb_ep_stall_get(usb_device_t *dev, uint8_t ep)
{
    (void)dev;

    if (ep & 0x80) {
        if ((*USB_EPnR(ep) & USB_EPTX_STAT) == USB_EP_TX_STALL) {
            return true;
        }
    } else {
        if ((*USB_EPnR(ep) & USB_EPRX_STAT) == USB_EP_RX_STALL) {
            return true;
        }
    }

    return false;
}

static void st_usb_ep_stall_set(usb_device_t *dev, uint8_t ep, bool stall)
{
    (void) dev;

    if (ep == 0) {
        USB_SET_EP_TX_STAT(ep, stall ? USB_EP_TX_STALL : USB_EP_TX_NAK);
    }

    if (ep & 0x80) {
        ep &= 0x7F;

        USB_SET_EP_TX_STAT(ep, stall ? USB_EP_TX_STALL : USB_EP_TX_NAK);

        /* Reset to DATA0 if clearing stall condition. */
        if (!stall) {
            USB_CLR_EP_TX_DTOG(ep);
        }
    } else {
        /* Reset to DATA0 if clearing stall condition. */
        if (!stall) {
            USB_CLR_EP_RX_DTOG(ep);
        }

        USB_SET_EP_RX_STAT(ep, stall ? USB_EP_RX_STALL : USB_EP_RX_VALID);
    }
}

static void st_usb_ep_nak_set(usb_device_t *dev, uint8_t ep, bool nak)
{
    (void)dev;
    /* It does not make sense to force NAK on IN endpoints. */
    if (ep & 0x80) {
        return;
    }

    _force_nak[ep] = nak;

    if (nak) {
        USB_SET_EP_RX_STAT(ep, USB_EP_RX_NAK);
    } else {
        USB_SET_EP_RX_STAT(ep, USB_EP_RX_VALID);
    }
}

static uint16_t st_usb_ep_wr_packet(usb_device_t *dev, uint8_t ep, const void *buf, uint16_t len)
{
    (void) dev;
    ep &= 0x7F;

    if ((*USB_EPnR(ep) & USB_EPTX_STAT) == USB_EP_TX_VALID) {
        return 0;
    }

    st_usb_buf_to_pma(buf, USB_GET_EP_TX_ADDR(ep), len);
    USB_SET_EP_TX_COUNT(ep, len);
    USB_SET_EP_TX_STAT(ep, USB_EP_TX_VALID);

    return len;
}

static uint16_t st_usb_ep_rd_packet(usb_device_t *dev, uint8_t ep, void *buf, uint16_t len)
{
    (void)dev;

    if ((*USB_EPnR(ep) & USB_EPRX_STAT) == USB_EP_RX_VALID) {
        return 0;
    }

    len = MIN(USB_GET_EP_RX_COUNT(ep), len);
    st_usb_pma_to_buf(buf, USB_GET_EP_RX_ADDR(ep), len);
    USB_CLR_EP_RX_CTR(ep);

    if (!_force_nak[ep]) {
        USB_SET_EP_RX_STAT(ep, USB_EP_RX_VALID);
    }

    return len;
}

static void st_usb_poll(usb_device_t *dev)
{
    uint16_t istr = USB->ISTR;

    if (istr & USB_ISTR_RESET) {
        USB->ISTR &= ~USB_ISTR_RESET;
        dev->pm_top = USBD_PM_TOP;
        usbd_reset(dev);
        return;
    }

    if (istr & USB_ISTR_CTR) {
        uint8_t ep = istr & USB_ISTR_EP_ID;
        if (ep == 0) {
            if (istr & USB_ISTR_DIR) {
                if (USB->EP0R & USB_EP_SETUP) {
                    usbd_ep0_setup(dev);
                } else {
                    usbd_ep0_out(dev);
                }
            } else {
                USB_CLR_EP_TX_CTR(0);
                usbd_ep0_in(dev);
            }
        } else {
            if (*USB_EPnR(ep) & USB_EP_CTR_RX) {
                USB_CLR_EP_RX_CTR(ep);
                if (dev->cb_endpoints[ep - 1][USB_TRANSACTION_OUT]) {
                    dev->cb_endpoints[ep - 1][USB_TRANSACTION_OUT](dev, ep);
                }
            }
            if (*USB_EPnR(ep) & USB_EP_CTR_TX) {
                USB_CLR_EP_TX_CTR(ep);
                if (dev->cb_endpoints[ep - 1][USB_TRANSACTION_IN]) {
                    dev->cb_endpoints[ep - 1][USB_TRANSACTION_IN](dev, ep);
                }
            }
        }
    }

    if (istr & USB_ISTR_DOVR) {
        USB->ISTR &= ~USB_ISTR_DOVR;
        if (dev->cb_dovr) {
            dev->cb_dovr();
        }
    }

    if (istr & USB_ISTR_ERR) {
        USB->ISTR &= ~USB_ISTR_ERR;
        if (dev->cb_error) {
            dev->cb_error();
        }
    }

    if (istr & USB_ISTR_WKUP) {
        USB->ISTR &= ~USB_ISTR_WKUP;
        if (dev->cb_wakeup) {
            dev->cb_wakeup();
        }
    }

    if (istr & USB_ISTR_SUSP) {
        USB->ISTR &= ~USB_ISTR_SUSP;
        if (dev->cb_suspend) {
            dev->cb_suspend();
        }
    }

    if (istr & USB_ISTR_SOF) {
        USB->ISTR &= ~USB_ISTR_SOF;
        if (dev->cb_sof) {
            dev->cb_sof();
        }
    }

    if (dev->cb_sof) {
        USB->CNTR |= USB_CNTR_SOFM;
    } else {
        USB->CNTR &= ~USB_CNTR_SOFM;
    }
}

usb_driver_t st_usb_driver = {
    .init = st_usb_init,
    .set_address = st_usb_set_address,
    .ep_setup = st_usb_ep_setup,
    .ep_reset = st_usb_ep_reset,
    .ep_stall_get = st_usb_ep_stall_get,
    .ep_stall_set = st_usb_ep_stall_set,
    .ep_nak_set = st_usb_ep_nak_set,
    .ep_wr_packet = st_usb_ep_wr_packet,
    .ep_rd_packet = st_usb_ep_rd_packet,
    .poll = st_usb_poll,
};