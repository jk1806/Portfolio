/**
 * DFU (Device Firmware Update) Implementation
 * Author: jk1806
 * Created: 2024-04-18
 * 
 * USB Device Firmware Upgrade protocol implementation
 * Supports firmware updates via USB interface
 */

#include <linux/usb/composite.h>
#include <linux/usb/gadget.h>
#include <linux/usb/dfu.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#define DFU_VERSION "1.1.0"
#define DFU_BLOCK_SIZE 1024
#define DFU_MAX_TRANSFER_SIZE (64 * 1024)

struct dfu_function {
    struct usb_function function;
    struct usb_ep *in_ep;
    struct usb_ep *out_ep;
    u8 state;
    u8 status;
    u16 block_num;
    u16 block_size;
    u32 transfer_size;
    void *firmware_data;
    size_t firmware_size;
    size_t firmware_offset;
};

static struct dfu_function *dfu_func;

/**
 * DFU state machine states
 */
enum dfu_state {
    DFU_STATE_appIDLE = 0,
    DFU_STATE_appDETACH,
    DFU_STATE_dfuIDLE,
    DFU_STATE_dfuDNLOAD_SYNC,
    DFU_STATE_dfuDNBUSY,
    DFU_STATE_dfuDNLOAD_IDLE,
    DFU_STATE_dfuMANIFEST_SYNC,
    DFU_STATE_dfuMANIFEST,
    DFU_STATE_dfuMANIFEST_WAIT_RESET,
    DFU_STATE_dfuUPLOAD_IDLE,
    DFU_STATE_dfuERROR
};

/**
 * Handle DFU_DETACH request
 */
static int dfu_handle_detach(struct dfu_function *dfu, u16 timeout)
{
    pr_info("DFU: Detach request, timeout=%d ms\n", timeout);
    
    dfu->state = DFU_STATE_appDETACH;
    
    // Schedule reset after timeout
    schedule_delayed_work(&dfu->reset_work, msecs_to_jiffies(timeout));
    
    return 0;
}

/**
 * Handle DFU_DNLOAD request
 */
static int dfu_handle_dnload(struct dfu_function *dfu, u16 block_num,
                             const void *data, size_t len)
{
    int ret;
    
    pr_info("DFU: DNLOAD block %d, len %zu\n", block_num, len);
    
    if (block_num == 0) {
        // First block - initialize transfer
        if (dfu->firmware_data) {
            vfree(dfu->firmware_data);
        }
        
        dfu->firmware_size = 0;
        dfu->firmware_offset = 0;
        dfu->firmware_data = NULL;
        
        dfu->state = DFU_STATE_dfuDNLOAD_SYNC;
    }
    
    if (len == 0) {
        // End of transfer
        dfu->state = DFU_STATE_dfuMANIFEST_SYNC;
        pr_info("DFU: Transfer complete, %zu bytes\n", dfu->firmware_size);
        return 0;
    }
    
    // Allocate or reallocate buffer
    if (!dfu->firmware_data) {
        dfu->firmware_data = vmalloc(DFU_MAX_TRANSFER_SIZE);
        if (!dfu->firmware_data) {
            pr_err("DFU: Failed to allocate firmware buffer\n");
            dfu->state = DFU_STATE_dfuERROR;
            return -ENOMEM;
        }
    }
    
    // Copy data to buffer
    if (dfu->firmware_offset + len > DFU_MAX_TRANSFER_SIZE) {
        pr_err("DFU: Firmware too large\n");
        dfu->state = DFU_STATE_dfuERROR;
        return -ENOSPC;
    }
    
    memcpy(dfu->firmware_data + dfu->firmware_offset, data, len);
    dfu->firmware_offset += len;
    dfu->firmware_size = dfu->firmware_offset;
    
    dfu->state = DFU_STATE_dfuDNBUSY;
    
    // Simulate write operation
    msleep(10);
    
    dfu->state = DFU_STATE_dfuDNLOAD_IDLE;
    
    return 0;
}

/**
 * Handle DFU_UPLOAD request
 */
static int dfu_handle_upload(struct dfu_function *dfu, u16 block_num,
                             void *data, size_t *len)
{
    size_t offset;
    size_t copy_len;
    
    pr_info("DFU: UPLOAD block %d\n", block_num);
    
    if (!dfu->firmware_data || dfu->firmware_size == 0) {
        *len = 0;
        return 0;
    }
    
    offset = block_num * DFU_BLOCK_SIZE;
    if (offset >= dfu->firmware_size) {
        *len = 0;
        return 0;
    }
    
    copy_len = min_t(size_t, *len, dfu->firmware_size - offset);
    memcpy(data, dfu->firmware_data + offset, copy_len);
    *len = copy_len;
    
    return 0;
}

/**
 * Handle DFU_GETSTATUS request
 */
static void dfu_get_status(struct dfu_function *dfu, struct dfu_status *status)
{
    status->bStatus = dfu->status;
    status->bState = dfu->state;
    status->iString = 0;
    
    // Set poll timeout based on state
    switch (dfu->state) {
    case DFU_STATE_dfuDNBUSY:
        status->bwPollTimeout[0] = 10;  // 10ms
        break;
    case DFU_STATE_dfuMANIFEST_SYNC:
        status->bwPollTimeout[0] = 1000;  // 1s
        break;
    default:
        status->bwPollTimeout[0] = 0;
        break;
    }
}

/**
 * DFU control request handler
 */
static int dfu_setup(struct usb_function *f, const struct usb_ctrlrequest *ctrl)
{
    struct dfu_function *dfu = func_to_dfu(f);
    u16 w_value = le16_to_cpu(ctrl->wValue);
    u16 w_index = le16_to_cpu(ctrl->wIndex);
    u16 w_length = le16_to_cpu(ctrl->wLength);
    int ret = 0;
    
    if ((ctrl->bRequestType & USB_TYPE_MASK) != USB_TYPE_CLASS)
        return -EOPNOTSUPP;
    
    switch (ctrl->bRequest) {
    case USB_DFU_DETACH:
        ret = dfu_handle_detach(dfu, w_value);
        break;
    case USB_DFU_DNLOAD:
        ret = dfu_handle_dnload(dfu, w_index, NULL, w_length);
        break;
    case USB_DFU_UPLOAD:
        ret = dfu_handle_upload(dfu, w_index, NULL, &w_length);
        break;
    case USB_DFU_GETSTATUS: {
        struct dfu_status status;
        dfu_get_status(dfu, &status);
        memcpy(req->buf, &status, sizeof(status));
        ret = sizeof(status);
        break;
    }
    case USB_DFU_CLRSTATUS:
        dfu->status = 0;
        dfu->state = DFU_STATE_dfuIDLE;
        break;
    case USB_DFU_GETSTATE:
        *((u8 *)req->buf) = dfu->state;
        ret = 1;
        break;
    case USB_DFU_ABORT:
        dfu->state = DFU_STATE_dfuIDLE;
        break;
    default:
        ret = -EOPNOTSUPP;
        break;
    }
    
    return ret;
}

