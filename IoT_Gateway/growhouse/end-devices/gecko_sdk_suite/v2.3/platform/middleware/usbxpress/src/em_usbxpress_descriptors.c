/***************************************************************************//**
 * @file    em_usbxpress_descriptors.c
 * @brief   Initial USB Descriptors.
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "usbconfig.h"
#include "em_usbxpress_internal.h"
#include "em_usbxpress_descriptors.h"

// -----------------------------------------------------------------------------
// USB Initial Descriptors

/// USB Device Descriptor
SL_ALIGN(4)
const USB_DeviceDescriptor_TypeDef
USBXCORE_deviceDescInit SL_ATTRIBUTE_ALIGN(4) =
{
  .bLength            = USB_DEVICE_DESCSIZE,
  .bDescriptorType    = USB_DEVICE_DESCRIPTOR,
  .bcdUSB             = 0x0200,
  .bDeviceClass       = 0,
  .bDeviceSubClass    = 0,
  .bDeviceProtocol    = 0,
  .bMaxPacketSize0    = USB_FS_CTRL_EP_MAXSIZE,
  .idVendor           = 0x10C4,
  .idProduct          = 0x0000,
  .bcdDevice          = 0x0100,
  .iManufacturer      = 1,
  .iProduct           = 2,
  .iSerialNumber      = 3,
  .bNumConfigurations = 1
};

/// USB Configuration Descriptor
SL_ALIGN(4)
const uint8_t USBXCORE_configDescInit[] SL_ATTRIBUTE_ALIGN(4) =
{
  // Configuration descriptor
  USB_CONFIG_DESCSIZE,              // bLength
  USB_CONFIG_DESCRIPTOR,            // bDescriptorType

  USB_CONFIG_DESCSIZE               // wTotalLength (LSB)
  + USB_INTERFACE_DESCSIZE
  + (USB_ENDPOINT_DESCSIZE * NUM_EP_USED),

  (USB_CONFIG_DESCSIZE              // wTotalLength (MSB)
   + USB_INTERFACE_DESCSIZE
   + (USB_ENDPOINT_DESCSIZE * NUM_EP_USED)) >> 8,

  1,                                // bNumInterfaces
  1,                                // bConfigurationValue
  0,                                // iConfiguration

#if defined(SLAB_USB_BUS_POWERED)
  CONFIG_DESC_BM_RESERVED_D7,       // bmAttrib: Bus-powered
#else
  CONFIG_DESC_BM_RESERVED_D7        // bmAttrib: Self-powered
  | CONFIG_DESC_BM_SELFPOWERED,
#endif
  CONFIG_DESC_MAXPOWER_mA(100),     // bMaxPower: 100 mA

  // Interface descriptor
  USB_INTERFACE_DESCSIZE,           // bLength
  USB_INTERFACE_DESCRIPTOR,         // bDescriptorType
  USBXPRESS_IFC_NUMBER,             // bInterfaceNumber
  0,                                // bAlternateSetting
  2,                                // bNumEndpoints
  0xFF,                             // bInterfaceClass (Vendor-Specific)
  0,                                // bInterfaceSubClass
  0,                                // bInterfaceProtocol
  0,                                // iInterface

  // Endpoint 1 IN descriptor
  USB_ENDPOINT_DESCSIZE,            // bLength
  USB_ENDPOINT_DESCRIPTOR,          // bDescriptorType
  0x81,                             // bEndpointAddress (IN)
  USB_EPTYPE_BULK,                  // bmAttributes
  USB_FS_BULK_EP_MAXSIZE,           // wMaxPacketSize (LSB)
  0,                                // wMaxPacketSize (MSB)
  0,                                // bInterval

  // Endpoint 1 OUT descriptor
  USB_ENDPOINT_DESCSIZE,            // bLength
  USB_ENDPOINT_DESCRIPTOR,          // bDescriptorType
  0x01,                             // bEndpointAddress (OUT)
  USB_EPTYPE_BULK,                  // bmAttributes
  USB_FS_BULK_EP_MAXSIZE,           // wMaxPacketSize (LSB)
  0,                                // wMaxPacketSize (MSB)
  0,                                // bInterval
};

/// USB Language String Descriptor
SL_ALIGN(4)
const uint8_t USBXCORE_stringLangDesc[] SL_ATTRIBUTE_ALIGN(4) =
{
  // English String Descriptor
  0x04, 0x03, 0x09, 0x04
};

/// USB Buffer Multiplier
// Endpoint buffer sizes
// 1 = single buffer, 2 = double buffering, 3 = triple buffering ...
SL_ALIGN(4)
const uint8_t USBXCORE_buffMult[NUM_EP_USED + 1] SL_ATTRIBUTE_ALIGN(4) =
{ 1, 2, 2 };

/// Microsoft OS Descriptor
#define MSOS_DESC_LEN sizeof ("MSFT100\x01") * 2
SL_ALIGN(4)
const uint8_t USBXCORE_microsoftOsDesc[] SL_ATTRIBUTE_ALIGN(4) =
{
  MSOS_DESC_LEN, 0x03,
  'M', 0,
  'S', 0,
  'F', 0,
  'T', 0,
  '1', 0,
  '0', 0,
  '0', 0,
  EXT_COMP_VENDOR_CODE, 0,
};

/// Compatible ID Feature Descriptor
SL_ALIGN(4)
const USB_ExtendedCompatIdOsFeatureDesc_t
USBXCORE_extendedCompatIdOsFeatureDesc SL_ATTRIBUTE_ALIGN(4) =
{
  EXT_COMP_DESC_SIZE,                         // Size
  0x0100,                                     // bcdVersion
  FEATURE_EXTENDED_COMPATIBILITY_ID,          // Command Index
  NUM_IFCS_WITH_EXT_COMPAT,                   // Number of interfaces with
                                              // extended compatibility
  { 0, 0, 0, 0, 0, 0, 0 },                    // Reserved (unused)

  { // USB_Extended Compatibility Interface Descriptor
    {
      0,                                      // Interface Number
      1,                                      // Reserved (unused)
      "WINUSB\0",                             // Compatible ID String
      { 0, 0, 0, 0, 0, 0, 0, 0 },             // Sub-compatible ID String
      { 0, 0, 0, 0, 0, 0 },                   // Reserved (unused)
    },
  }
};

/// Extended Properties Descriptor
SL_ALIGN(4)
const USB_ExtendedPropertiesDescriptor_t
USBXCORE_extendedPropertiesDesc SL_ATTRIBUTE_ALIGN(4) =
{
  // Header
  PROPERTIES_DESCRIPTOR_SIZE,               // dwLength
  0x0100,                                   // bcdVersion: 1.0
  FEATURE_EXTENDED_PROPERTIES_ID,           // wIndex: Extended Properties ID
  1,                                        // wCount: 1 Custom Property

  // GUID Property
  GUID_PROPERTY_DESCRIPTOR_SIZE,            // dwSize
  EXTENDED_PROPERTY_TYPE_MULTI_SZ,          // dwPropertyDataType
  GUID_PROPERTY_NAME_LEN,                   // wPropertyNameLength
  GUID_PROPERTY_NAME,                       // bPropertyName
  GUID_PROPERTY_LEN,                        // dwPropertyDataLength
  GUID_PROPERTY,                            // bPropertyData
};
