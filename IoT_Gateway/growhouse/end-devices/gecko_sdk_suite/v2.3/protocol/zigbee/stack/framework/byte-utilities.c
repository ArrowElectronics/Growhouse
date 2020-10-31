/*
 * File: byte-utilities.c
 * Description: Data store and fetch routines.
 *
 * Copyright 2014 Silicon Laboratories, Inc.                                *80*
 */

#include PLATFORM_HEADER

// Copy bytes from source to destination, reversing the order.
void emberReverseMemCopy(uint8_t* dest, const uint8_t* src, uint8_t length)
{
  uint8_t i;
  uint8_t j = (length - 1);

  for ( i = 0; i < length; i++) {
    dest[i] = src[j];
    j--;
  }
}

uint16_t emberFetchLowHighInt16u(const uint8_t *contents)
{
  return HIGH_LOW_TO_INT(contents[1], contents[0]);
}

uint16_t emberFetchHighLowInt16u(const uint8_t *contents)
{
  return HIGH_LOW_TO_INT(contents[0], contents[1]);
}

void emberStoreLowHighInt16u(uint8_t *contents, uint16_t value)
{
  contents[0] = LOW_BYTE(value);
  contents[1] = HIGH_BYTE(value);
}

void emberStoreHighLowInt16u(uint8_t *contents, uint16_t value)
{
  contents[0] = HIGH_BYTE(value);
  contents[1] = LOW_BYTE(value);
}

void emStoreInt32u(bool lowHigh, uint8_t* contents, uint32_t value)
{
  uint8_t ii;
  for (ii = 0; ii < 4; ii++) {
    uint8_t index = (lowHigh ? ii : 3 - ii);
    contents[index] = (uint8_t)(value & 0xFF);
    value = (value >> 8);
  }
}

uint32_t emFetchInt32u(bool lowHigh, const uint8_t* contents)
{
  uint8_t b0;
  uint8_t b1;
  uint8_t b2;
  uint8_t b3;

  if (lowHigh) {
    b0 = contents[3];
    b1 = contents[2];
    b2 = contents[1];
    b3 = contents[0];
  } else {
    b0 = contents[0];
    b1 = contents[1];
    b2 = contents[2];
    b3 = contents[3];
  }
  return ((((uint32_t) ((uint16_t) (((uint16_t) b0) << 8) | ((uint16_t) b1)))
           << 16)
          | (uint32_t) ((uint16_t) (((uint16_t) b2) << 8) | ((uint16_t) b3)));
}
