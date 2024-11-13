#include <usb/class/usb_dfu.h>

#include <string.h>

#define DFU(ctx) ((dfu_t *) ctx)

__STATIC_FORCEINLINE
usb_result_t _dfu_upload(dfu_t *dfu, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    switch (dfu->state)
    {
        case DFU_STATE_DFU_IDLE: {
            dfu->offset = 0;
        }
        case DFU_STATE_DFU_UPLOAD_IDLE: {
            uint32_t max = dfu_get_upload_size();

            if (dfu->offset + req->wLength > max) {
                *len = max - dfu->offset;
                dfu->state = DFU_STATE_DFU_IDLE;
            } else {
                *len = req->wLength;
                dfu->state = DFU_STATE_DFU_UPLOAD_IDLE;
            }
            dfu->offset += *len;

            *buf = dfu_get_upload_ptr(dfu->offset);
            break;
        }
        default:
            break;
    }

    return USB_RESULT_HANDLED;
}

__STATIC_FORCEINLINE
usb_result_t _dfu_dnload(dfu_t *dfu, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    switch (dfu->state)
    {
        case DFU_STATE_DFU_IDLE: {
            dfu->offset = 0;
            /* Reset manifestation progress on new download */
            dfu->manifestation_complete = false;
        }
        case DFU_STATE_DFU_DNLOAD_IDLE: {
            dfu->block_num  = req->wValue;
            dfu->dnload_len = req->wLength;
            memcpy(dfu->dnload_buf, buf, req->wLength);
            dfu->state = DFU_STATE_DFU_DNLOAD_SYNC;
        }
        default:
            break;
    }
    return USB_RESULT_HANDLED;
}

__STATIC_FORCEINLINE
usb_result_t _dfu_get_state(dfu_t *dfu, uint8_t **buf, uint16_t *len)
{
    *buf[0] = dfu->state;
    *len = 1;
    return USB_RESULT_HANDLED;
}

__STATIC_FORCEINLINE
usb_result_t _dfu_get_status(dfu_t *dfu, uint8_t **buf, uint16_t *len)
{
    uint32_t bwPollTimeout = 0;

    switch (dfu->state)
    {
        case DFU_STATE_DFU_DNLOAD_SYNC:
            dfu->state = DFU_STATE_DFU_DNBUSY;
            bwPollTimeout = 100;
            break;
        case DFU_STATE_DFU_MANIFEST_SYNC:
            if (dfu->manifestation_complete) {// <-- this only if manifestation tolerant
                dfu->state = DFU_STATE_DFU_IDLE;
            } else {
                dfu->state = DFU_STATE_DFU_MANIFEST;
            }
            break;
        default:
            break;
    }

    *buf[0] = DFU_STATUS_OK;
    *buf[1] = bwPollTimeout & 0xFF;
    *buf[2] = (bwPollTimeout >> 8) & 0xFF;
    *buf[3] = (bwPollTimeout >> 16) & 0xFF;
    *buf[4] = dfu->state;
    *buf[5] = 0; /* iString not used here */
    *len = 6;
    return USB_RESULT_HANDLED;
}

static void ep0_status_out(void *ctx, usb_request_t *req)
{
    if (req->bRequest != DFU_REQUEST_GET_STATUS) {
        return;
    }

    dfu_t *dfu = DFU(ctx);

    switch (dfu->state)
    {
        case DFU_STATE_APP_DETACH: {
            dfu->cb_reset_to_dfu();
            break;
        }
        case DFU_STATE_DFU_DNBUSY: {
            dfu->cb_buf_to_mem(dfu->dnload_buf, dfu->dnload_len, dfu->block_num);
            dfu->state = DFU_STATE_DFU_DNLOAD_SYNC;
            break;
        }
        case DFU_STATE_DFU_MANIFEST: {
            dfu->manifestation_complete = true;
            //TODO check bitManifestationTolerant attr??? better exclude logic by ifdef
            if (true) {
                dfu->state = DFU_STATE_DFU_MANIFEST_SYNC;
            } else {
                dfu->state = DFU_STATE_DFU_MANIFEST_WAIT_RESET;
                dfu->cb_reset_to_app();
            }
            break;
        }
        default:
            break;
    }
}

static usb_result_t ep0_setup_no_data(void *ctx, usb_request_t *req)
{
    if ((req->bmRequestType & DFU_REQ_MASK) != DFU_REQ_TYPE) {
        return USB_RESULT_NOTSUPP;
    }

    dfu_t *dfu = DFU(ctx);

    switch (req->bRequest)
    {
        case DFU_REQUEST_DNLOAD: {
            /* End of DNLOAD operation*/
            if (dfu->state == DFU_STATE_DFU_DNLOAD_IDLE || dfu->state == DFU_STATE_DFU_IDLE) {
                dfu->manifestation_complete = false;//in progress
                dfu->state = DFU_STATE_DFU_MANIFEST_SYNC;
                return USB_RESULT_HANDLED;
            }
            break;
        }
        case DFU_REQUEST_UPLOAD: {
            dfu->state = DFU_STATE_DFU_IDLE;
            return USB_RESULT_HANDLED;
        }
        case DFU_REQUEST_CLR_STATUS: {
            if (dfu->state == DFU_STATE_DFU_ERROR) {
                dfu->state  = DFU_STATE_DFU_IDLE;
                dfu->status = DFU_STATUS_OK;
            } else {
                dfu->state  = DFU_STATE_DFU_ERROR;
                dfu->status = DFU_STATUS_ERR_UNKNOWN;
            }
            return USB_RESULT_HANDLED;
        }
        case DFU_REQUEST_ABORT: {
            switch (dfu->state)
            {
                case DFU_STATE_DFU_IDLE:
                case DFU_STATE_DFU_DNLOAD_SYNC:
                case DFU_STATE_DFU_DNLOAD_IDLE:
                case DFU_STATE_DFU_MANIFEST_SYNC:
                case DFU_STATE_DFU_UPLOAD_IDLE:
                    dfu->state  = DFU_STATE_DFU_IDLE;
                    dfu->status = DFU_STATUS_OK;
                    break;
                default:
                    break;
            }
            return USB_RESULT_HANDLED;
        }
        case DFU_REQUEST_DETACH: {
            dfu->state  = DFU_STATE_APP_DETACH;
            return USB_RESULT_HANDLED;
        }
        default:
            break;
    }

    return USB_RESULT_NOTSUPP;
}

static usb_result_t ep0_setup_data(void *ctx, usb_request_t *req, uint8_t **buf, uint16_t *len)
{
    if ((req->bmRequestType & DFU_REQ_MASK) != DFU_REQ_TYPE) {
        return USB_RESULT_NOTSUPP;
    }

    dfu_t *dfu = DFU(ctx);

    switch (req->bRequest)
    {
        case DFU_REQUEST_UPLOAD: {
            return _dfu_upload(dfu, req, buf, len);
        }
        case DFU_REQUEST_DNLOAD: {
            return _dfu_dnload(dfu, req, buf, len);
        }
        case DFU_REQUEST_GET_STATE: {
            return _dfu_get_state(dfu, buf, len);
        }
        case DFU_REQUEST_GET_STATUS: {
            return _dfu_get_status(dfu, buf, len);
        }
    }

    return USB_RESULT_NOTSUPP;
}

usb_interface_t *dfu_setup(usb_interface_t *interface)
{
    interface->ep0_status_out = ep0_status_out;
    interface->ep0_setup_rd = ep0_setup_no_data;
    interface->ep0_setup_wr = ep0_setup_data;
    return interface;
}
