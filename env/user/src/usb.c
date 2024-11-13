//TODO descriptors: cdc, dfu

#include <usb/usb_descriptors.h>
#include <usb/class/usb_dfu.h>
#include <usb/class/usb_cdc.h>

//TODO composite???
//https://github.com/libopencm3/libopencm3-examples/issues/242

static const usb_endpoint_descriptor_t cdc_comm_endpoints[] = {
    {
        .bLength = USB_DESCRIPTOR_SIZE_ENDPOINT,
        .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = 0x83,
        .bmAttributes = USB_ENDPOINT_TRANSFER_TYPE_INTERRUPT,
        .wMaxPacketSize = 16,
        .bInterval = 255,
    }
};

static const usb_endpoint_descriptor_t cdc_data_endpoints[] = {
    {
        .bLength = USB_DESCRIPTOR_SIZE_ENDPOINT,
        .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = 0x01,
        .bmAttributes = USB_ENDPOINT_TRANSFER_TYPE_BULK,
        .wMaxPacketSize = 64,
        .bInterval = 1,
    }, 
    {
        .bLength = USB_DESCRIPTOR_SIZE_ENDPOINT,
        .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = 0x82,
        .bmAttributes = USB_ENDPOINT_TRANSFER_TYPE_BULK,
        .wMaxPacketSize = 64,
        .bInterval = 1,
    }
};

static const struct {
    cdc_header_functional_descriptor_t cdc_header;
    cdc_cm_functional_descriptor_t cdc_cm;
    cdc_acm_functional_descriptor_t cdc_acm;
    cdc_union_functional_descriptor_t cdc_union;
} __attribute__((packed)) cdc_functional_descriptors = {
    .cdc_header = {
        .bFunctionLength = sizeof(cdc_header_functional_descriptor_t),
        .bDescriptorType = CDC_DESCRIPTOR_TYPE_CS_INTERFACE,
        .bDescriptorSubtype = CDC_DESCRIPTOR_SUBTYPE_HEADER,
        .bcdCDC = 0x0110,
    },
    .cdc_cm = {
        .bFunctionLength = sizeof(cdc_cm_functional_descriptor_t),
        .bDescriptorType = CDC_DESCRIPTOR_TYPE_CS_INTERFACE,
        .bDescriptorSubtype = CDC_DESCRIPTOR_SUBTYPE_CM,
        .bmCapabilities = 0,
        .bDataInterface = 1,
    },
    .cdc_acm = {
        .bFunctionLength = sizeof(cdc_acm_functional_descriptor_t),
        .bDescriptorType = CDC_DESCRIPTOR_TYPE_CS_INTERFACE,
        .bDescriptorSubtype = CDC_DESCRIPTOR_SUBTYPE_ACM,
        .bmCapabilities = 0,
    },
    .cdc_union = {
        .bFunctionLength = sizeof(cdc_union_functional_descriptor_t),
        .bDescriptorType = CDC_DESCRIPTOR_TYPE_CS_INTERFACE,
        .bDescriptorSubtype = CDC_DESCRIPTOR_SUBTYPE_UNION,
        .bControlInterface = 0,
        .bSubordinateInterface0 = 1, 
    }
};

static const usb_interface_descriptor_t cdc_comm_interface = {
    .bLength = USB_DESCRIPTOR_SIZE_INTERFACE,
    .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
    .bInterfaceNumber = 0,
    .bAlternateSetting = 0,
    .bNumEndpoints = 1,
    .bInterfaceClass = USB_CLASS_COMMUNICATION,
    .bInterfaceSubClass = CDC_CC_SUBCLASS_ACM,
    .bInterfaceProtocol = CDC_CC_PROTOCOL_AT,
    .iInterface = 0,

    .endpoint = cdc_comm_endpoints,

    .extra_ptr = &cdc_functional_descriptors,
    .extra_len = sizeof(cdc_functional_descriptors)
};

static const usb_interface_descriptor_t cdc_data_interface = {
    .bLength = USB_DESCRIPTOR_SIZE_INTERFACE,
    .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
    .bInterfaceNumber = 1,
    .bAlternateSetting = 0,
    .bNumEndpoints = 2,
    .bInterfaceClass = USB_CLASS_CDC_DATA,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    .iInterface = 0,

    .endpoint = cdc_data_endpoints,
};

static const dfu_functional_descriptor_t dfu_function = {
    .bLength = sizeof(dfu_functional_descriptor_t),
    .bDescriptorType = DFU_DESCRIPTOR_TYPE_FUNCTIONAL,
    .bmAttributes = 0,
    .wDetachTimeout = 255,
    .wTransferSize = 1024,
    .bcdDFUVersion = 0x011A,
};

static const usb_interface_descriptor_t dfu_interface = {
    .bLength = USB_DESCRIPTOR_SIZE_INTERFACE,
    .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
    .bInterfaceNumber = 0,
    .bAlternateSetting = 0,
    .bNumEndpoints = 0,
    .bInterfaceClass = USB_CLASS_APPLICATION_SPECIFIC, /* Device Firmware Upgrade */
    .bInterfaceSubClass = 1,
    .bInterfaceProtocol = 2,
    .iInterface = 4,

    .extra_ptr = &dfu_function,
    .extra_len = sizeof(dfu_function),
};

static const usb_interface_t interfaces[] = {
    {
        .num_altsetting = 1,
        .altsettings = &cdc_comm_interface,
    },
    {
        .num_altsetting = 1,
        .altsettings = &cdc_data_interface,
    },
    {
        .num_altsetting = 1,
        .altsettings = &dfu_interface,
    }
};

static const usb_config_descriptor_t config = {
    .bLength = USB_DESCRIPTOR_SIZE_CONFIG,
    .bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIG,
    .wTotalLength = 0,
    .bNumInterfaces = 3,
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = USB_CONFIG_ATTR_DEFAULT,
    .bMaxPower = 0x32,

    .interfaces = interfaces,
};

static const usb_device_descriptor_t device = {
    .bLength = USB_DESCRIPTOR_SIZE_DEVICE,
    .bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = USB_CLASS_DEFINED_IN_INTERFACE,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = 64,
    .idVendor = USB_VID,
    .idProduct = USB_VID,
    .bcdDevice = 0x0200,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1,
};

static const char *usb_strings[] = {
    "Black Sphere Technologies",
    "CDC-ACM Demo",
    "DEMO",
};
