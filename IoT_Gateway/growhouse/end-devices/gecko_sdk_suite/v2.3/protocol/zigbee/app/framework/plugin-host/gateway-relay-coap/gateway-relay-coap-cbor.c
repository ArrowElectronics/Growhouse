// Copyright 2017 Silicon Laboratories, Inc.                                *80*

// ************** simple CBOR encoder/decoder ***************
#include PLATFORM_HEADER //compiler/micro specifics, types

#include "app/framework/include/af.h"
#include "stack/include/ember-types.h"
#include "stack/include/error.h"

#include "hal/hal.h"
#include "app/framework/util/common.h"
#include "app/framework/plugin/device-table/device-table.h"
#include <string.h>
#include "app/framework/plugin-host/coap-server/coap-server.h"
#include "app/framework/plugin/device-table/device-table.h"
#include "gateway-relay-coap-cbor.h"

// Simple CBOR decoder for dotdot.  It assumes the arguments are in a CBOR map
static uint8_t *cborReadBuffer;
static uint16_t cborReadFinger;
static uint16_t cborReadBufferLength = 0;
static uint32_t cborNumArguments;
static uint8_t  cborArgFinger;

#define CBOR_TYPE_UNSIGNED_INT 0
#define CBOR_TYPE_NEGATIVE_INT 1
#define CBOR_TYPE_BYTE_STRING  2
#define CBOR_TYPE_TEXT_STRING  3
#define CBOR_TYPE_ARRAY        4
#define CBOR_TYPE_MAP          5

#define CBOR_EXTEND_1          24
#define CBOR_EXTEND_2          25
#define CBOR_EXTEND_4          26
#define CBOR_EXTEND_8          27

#define CBOR_FALSE  0xF4
#define CBOR_TRUE   0xF5
#define CBOR_NULL   0xF6
#define CBOR_BREAK  0xff

#define CBOR_TYPE_SHIFT  5
#define CBOR_LENGTH_MASK 0x1f
#define CBOR_LENGTH_INFINITE 0x1f
#define CBOR_MAX_IMPLEMENTED_LENGTH 4

#define MAX_STRING_LENGTH 256

static uint8_t grabTypeFromByte(uint8_t byte)
{
  return (byte >> CBOR_TYPE_SHIFT);
}

static uint8_t grabCborType(void)
{
  return (grabTypeFromByte(cborReadBuffer[cborReadFinger]));
}

static uint8_t grabLengthFromByte(uint8_t byte)
{
  return (byte & CBOR_LENGTH_MASK);
}

static EmberStatus cborGrabNextByte(uint8_t *byte)
{
  if (cborReadFinger < cborReadBufferLength) {
    *byte = cborReadBuffer[cborReadFinger];
    cborReadFinger++;
  } else {
    printf("CBOR: error:  reading past end of buffer.\r\n");
    return EMBER_ERR_FATAL;
  }

  return EMBER_SUCCESS;
}

static uint32_t makeValue(uint8_t length)
{
  uint32_t value = 0;
  int i;

  for (i = 0; i < length; i++) {
    value <<= 8;
    value += cborReadBuffer[cborReadFinger];
    cborReadFinger++;
  }

  return value;
}

// Note:  even though CBOR supports up to uint64_t bytes, we only support 32
// bits here.  We will throw an error otherwise.
static uint32_t grabCborValue(uint8_t *size)
{
  uint8_t shortValue;
  uint32_t fullValue = 0;

  // first byte is the initial length.
  shortValue = grabLengthFromByte(cborReadBuffer[cborReadFinger]);
  cborReadFinger += 1;

  switch (shortValue) {
    case CBOR_EXTEND_1:
      fullValue = makeValue(1);

      if (size != NULL) {
        *size = 1;
      }
      break;
    case CBOR_EXTEND_2:
      fullValue = makeValue(2);
      if (size != NULL) {
        *size = 2;
      }
      break;
    case CBOR_EXTEND_4:
      fullValue = makeValue(4);
      if (size != NULL) {
        *size = 4;
      }
      break;
    case CBOR_EXTEND_8:
      fprintf(stderr, "CBOR Error:  don't support 64 bit value\r\n");
      cborReadFinger += 8;
      fullValue = 0;
      if (size != NULL) {
        *size = 8;
      }
      break;
    default:
      fullValue = (uint32_t) shortValue;
      if (size != NULL) {
        *size = 1;
      }
      break;
  }
  return fullValue;
}

EmberStatus emAfPluginGatewayRelayCoapCborReadInit(uint8_t *buffer,
                                                   uint16_t bufferLength)
{
  uint8_t cborType;

  if (bufferLength == 0) {
    cborReadBufferLength = 0;
    return EMBER_SUCCESS;
  }

  cborReadBufferLength = bufferLength;
  cborReadBuffer = buffer;
  cborReadFinger = 0;

  cborType = grabTypeFromByte(cborReadBuffer[cborReadFinger]);

  if (cborType != CBOR_TYPE_MAP) {
    printf("ERROR:  wrong CBOR type %d\r\n", cborType);
    return EMBER_BAD_ARGUMENT;
  }

  cborNumArguments = grabCborValue(NULL);
  cborArgFinger = 0;

  return EMBER_SUCCESS;
}

EmberStatus emAfPluginGatewayRelayCoapCborGetUnsignedInteger(uint32_t *value,
                                                             uint8_t *size)
{
  if (cborArgFinger >= cborNumArguments
      || cborReadFinger >= cborReadBufferLength) {
    printf("CBOR: buffer overrun1\r\n");
    return EMBER_BAD_ARGUMENT;
  }

  if (grabCborType() != CBOR_TYPE_UNSIGNED_INT) {
    printf("CBOR:  expected unsigned int.  Got %d\r\n", grabCborType());
    return EMBER_BAD_ARGUMENT;
  }

  *value = grabCborValue(size);

  return EMBER_SUCCESS;
}

EmberStatus emAfPluginGatewayRelayCoapCborGetSignedInteger(int32_t *value,
                                                           uint8_t *size)
{
  uint8_t cborType = grabCborType();

  if (cborArgFinger >= cborNumArguments
      || cborReadFinger >= cborReadBufferLength) {
    return EMBER_BAD_ARGUMENT;
  }

  if (cborType != CBOR_TYPE_UNSIGNED_INT
      && cborType != CBOR_TYPE_NEGATIVE_INT) {
    return EMBER_BAD_ARGUMENT;
  }

  *value = grabCborValue(size);

  if (cborType == CBOR_TYPE_NEGATIVE_INT) {
    *value = *value * -1;
    *value -= 1;
  }

  return EMBER_SUCCESS;
}

EmberStatus emAfPluginGatewayRelayCoapCborGetByteStringDefiniteLength(
  uint8_t *buffer,
  uint16_t bufferLength)
{
  uint16_t i;
  uint32_t stringLength;
  uint8_t size;

  stringLength = grabCborValue(&size);

  if (size > CBOR_MAX_IMPLEMENTED_LENGTH || stringLength > bufferLength) {
    printf("CBOR:  error:  string length too long\r\n");
    return EMBER_BAD_ARGUMENT;
  }

  for (i = 0; i < stringLength; i++) {
    if (cborGrabNextByte(buffer)) {
      return EMBER_ERR_FATAL;
    }
    buffer++;
  }
  return EMBER_SUCCESS;
}

EmberStatus emAfPluginGatewayRelayCoapCborGetByteStringIndefiniteLength(
  uint8_t *buffer,
  uint16_t bufferLength)
{
  // Note:  this is a placeholder function for now.  CBOR standard allows an
  // indefinite lenght string.  I have chosen not to implement this yet
  // because I don't think we need it yet.
  return EMBER_ERR_FATAL;
}

EmberStatus emAfPluginGatewayRelayCoapCborGetBooleanValue(bool *value)
{
  uint8_t byte;

  if (cborGrabNextByte(&byte)) {
    return EMBER_ERR_FATAL;
  }

  if (byte == CBOR_TRUE) {
    *value = true;
  } else if (byte == CBOR_FALSE) {
    *value = false;
  } else {
    printf("CBOR: error:  expected boolen.  Got %02x\r\n", byte);
    return EMBER_BAD_ARGUMENT;
  }
  return EMBER_SUCCESS;
}

// ************** simple CBOR encoder ***************
// Simple CBOR encoder for Thread.  It assumes the arguments are in a CBOR map
static uint8_t *cborWriteBuffer;
static uint16_t cborWriteBufferLength;
static uint16_t cborWriteFinger;

EmberStatus emAfPluginGatewayRelayCoapCborWriteInit(uint8_t *buffer,
                                                    uint16_t bufferLength)
{
  cborWriteBuffer = buffer;
  cborWriteBufferLength = bufferLength;
  cborWriteFinger = 0;

  return EMBER_SUCCESS;
}

static EmberStatus cborWriteBufferByte(uint8_t byte)
{
  if (cborWriteFinger < cborWriteBufferLength) {
    cborWriteBuffer[cborWriteFinger] = byte;
    cborWriteFinger++;

    return EMBER_SUCCESS;
  } else {
    printf("ERROR:  CBOR buffer out of space\r\n");
    return EMBER_BAD_ARGUMENT;
  }
}

EmberStatus emAfPluginGatewayRelayCoapCborWriteMapTag(void)
{
  // write array tag with indefinite length (31)
  return cborWriteBufferByte((CBOR_TYPE_MAP << CBOR_TYPE_SHIFT)
                             + CBOR_LENGTH_INFINITE);
}

EmberStatus emAfPluginGatewayRelayCoapCborWriteArrayTag(void)
{
  // write array tag with indefinite length (31)
  return cborWriteBufferByte((CBOR_TYPE_ARRAY << CBOR_TYPE_SHIFT)
                             + CBOR_LENGTH_INFINITE);
}

uint16_t emAfPluginGatewayRelayCoapCborCurrentWriteDataLength(void)
{
  return cborWriteFinger;
}

EmberStatus emAfPluginGatewayRelayCoapCborWriteString(char *string)
{
  uint8_t i;
  uint16_t length;
  EmberStatus status;

  length = strlen(string);

  // Note:  does not handle strings greater than 256, which shouldn't be a
  // problem
  if (length < CBOR_EXTEND_1) {
    cborWriteBufferByte((CBOR_TYPE_TEXT_STRING << CBOR_TYPE_SHIFT) + (uint8_t) length);
  } else if (length < MAX_STRING_LENGTH) {
    cborWriteBufferByte((CBOR_TYPE_TEXT_STRING << CBOR_TYPE_SHIFT) + CBOR_EXTEND_1);
    cborWriteBufferByte((uint8_t) length);
  } else {
    return EMBER_BAD_ARGUMENT;
  }

  for (i = 0; i < length; i++) {
    status = cborWriteBufferByte((uint8_t) string[i]);
    if (status > 0) {
      return status;
    }
  }
  return EMBER_SUCCESS;
}

EmberStatus emAfPluginGatewayRelayCoapCborWriteBreak(void)
{
  return cborWriteBufferByte(CBOR_BREAK);
}

EmberStatus emAfPluginGatewayRelayCoapCborWriteTrue(void)
{
  return cborWriteBufferByte(CBOR_TRUE);
}

EmberStatus emAfPluginGatewayRelayCoapCborWriteFalse(void)
{
  return cborWriteBufferByte(CBOR_FALSE);
}

EmberStatus emAfPluginGatewayRelayCoapCborWriteInt8(int8_t signedData8)
{
  int32_t signedData32 = (int32_t) signedData8;

  emAfPluginGatewayRelayCoapCborWriteInt32(signedData32);

  return EMBER_SUCCESS;
}

EmberStatus emAfPluginGatewayRelayCoapCborWriteInt16(int16_t signedData16)
{
  int32_t signedData32 = (int32_t) signedData16;

  emAfPluginGatewayRelayCoapCborWriteInt32(signedData32);

  return EMBER_SUCCESS;
}

EmberStatus emAfPluginGatewayRelayCoapCborWriteUint8(uint8_t data8)
{
  uint32_t data32 = (uint32_t) data8;

  emAfPluginGatewayRelayCoapCborWriteUint32(data32);

  return EMBER_SUCCESS;
}

EmberStatus emAfPluginGatewayRelayCoapCborWriteUint16(uint16_t data16)
{
  uint32_t data32 = (uint32_t) data16;

  emAfPluginGatewayRelayCoapCborWriteUint32(data32);

  return EMBER_SUCCESS;
}

EmberStatus emAfPluginGatewayRelayCoapCborWriteInteger(uint32_t data32, uint8_t cborType)
{
  uint8_t byteValue;
  uint8_t i;
  EmberStatus status;

  // Note:  don't handle 8 byte values
  if (data32 < CBOR_EXTEND_1) {
    byteValue = (uint8_t) data32;
    status = cborWriteBufferByte((cborType << CBOR_TYPE_SHIFT) + byteValue);
  } else if (data32 < 0x100) {
    status = cborWriteBufferByte((cborType << CBOR_TYPE_SHIFT)
                                 + CBOR_EXTEND_1);
    byteValue = (uint8_t) data32;
    status = cborWriteBufferByte(byteValue);
  } else if (data32 < 0x10000) {
    status = cborWriteBufferByte((cborType << CBOR_TYPE_SHIFT)
                                 + CBOR_EXTEND_2);
    byteValue = (uint8_t) (data32 >> 8);
    cborWriteBufferByte(byteValue);
    byteValue = (uint8_t) (data32 & 0xff);
    cborWriteBufferByte(byteValue);
  } else {
    status = cborWriteBufferByte((cborType << CBOR_TYPE_SHIFT)
                                 + CBOR_EXTEND_4);
    byteValue = (uint8_t) (data32 >> 24);
    cborWriteBufferByte(byteValue);
    byteValue = (uint8_t) ((data32 >> 16) & 0xff);
    cborWriteBufferByte(byteValue);
    byteValue = (uint8_t) ((data32 >> 8) & 0xff);
    cborWriteBufferByte(byteValue);
    byteValue = (uint8_t) (data32 & 0xff);
    cborWriteBufferByte(byteValue);
  }
  return status;
}

EmberStatus emAfPluginGatewayRelayCoapCborWriteInt32(int32_t signedData32)
{
  uint32_t data32;
  uint8_t cborType = CBOR_TYPE_UNSIGNED_INT;

  if (signedData32 < 0) {
    signedData32 += 1;
    signedData32 *= -1;
    cborType = CBOR_TYPE_NEGATIVE_INT;
  }

  data32 = (uint32_t) signedData32;

  return emAfPluginGatewayRelayCoapCborWriteInteger(data32, cborType);
}

EmberStatus emAfPluginGatewayRelayCoapCborWriteUint32(uint32_t data32)
{
  return emAfPluginGatewayRelayCoapCborWriteInteger(data32, CBOR_TYPE_UNSIGNED_INT);
}
