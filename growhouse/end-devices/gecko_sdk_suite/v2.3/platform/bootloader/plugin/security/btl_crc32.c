/***************************************************************************//**
 * @file btl_crc32.c
 * @brief CRC32 functionality for Silicon Labs bootloader
 * @author Silicon Labs
 * @version 1.6.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#include "btl_crc32.h"
#include "em_device.h"

uint32_t btl_crc32Stream(const uint8_t *buffer,
                         size_t        length,
                         uint32_t      prevResult)
{
  CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_GPCRC;

  GPCRC->CTRL = GPCRC_CTRL_POLYSEL_CRC32 | GPCRC_CTRL_EN_ENABLE;
  GPCRC->INIT = prevResult;
  GPCRC->CMD = GPCRC_CMD_INIT;

  while (length--) {
    GPCRC->INPUTDATABYTE = *buffer++;
  }

  return GPCRC->DATA;
}
