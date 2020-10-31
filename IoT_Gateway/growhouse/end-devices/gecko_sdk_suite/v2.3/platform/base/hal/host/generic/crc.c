/** @file hal/host/generic/crc.c
 *  @brief  Generic firmware source for Cyclic Redundancy Check calculations.
 *
 * <!-- Copyright 2004-2010 by Ember Corporation. All rights reserved.   *80*-->
 */

#include PLATFORM_HEADER
#include "hal/host/crc.h"

/*
 *    16bit CRC notes:
 *    "CRC-CCITT"
 *     poly is g(X) = X^16 + X^12 + X^5 + 1  (0x1021)
 *     used in the FPGA (green boards and 15.4)
 *     initial remainder should be 0xFFFF
 */






uint16_t halCommonCrc16(uint8_t byte, uint16_t crc)
{
  crc = (crc >> 8) | (crc << 8);
  crc ^= byte;
  crc ^= (crc & 0xff) >> 4;
  crc ^= (crc << 8) << 4;






  crc ^= ((uint8_t) ((uint8_t) ((uint8_t) (crc & 0xff)) << 5))
         | ((uint16_t) ((uint8_t) ((uint8_t) (crc & 0xff)) >> 3) << 8);

  return crc;
}

//--------------------------------------------------------------
// CRC-32
#define POLYNOMIAL              (0xEDB88320L)

uint32_t halCommonCrc32(uint8_t data, uint32_t accumulator)
{
  uint8_t jj;
  uint32_t previous;
  uint32_t oper;

  previous = (accumulator >> 8) & 0x00FFFFFFL;
  oper = (accumulator ^ data) & 0xFF;
  for (jj = 0; jj < 8; jj++) {
    oper = ((oper & 0x01)
            ? ((oper >> 1) ^ POLYNOMIAL)
            : (oper >> 1));
  }

  return (previous ^ oper);
}
