/***************************************************************************//**
 * @file
 * @brief read_char.c
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "em_emu.h"
#include "retargetserial.h"

#include "read_char.h"

int32_t readChar()
{
  int32_t c = getchar();

#ifdef SERIAL_ECHO
  if ( (c != (int32_t)(-1)) && (c != (int32_t)(0)) ) {
    putchar(c);
  }
#endif

  return c;
}
