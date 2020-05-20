/******************************************************************************/
/**
 * @file spi-host.c
 * @brief SPI protocol Host functions
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/
#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "hal/hal.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/ezsp-host/ezsp-host-common.h"
#include "app/ezsp-host/spi/spi-host.h"
#include "app/ezsp-host/ezsp-host-io.h"
#include "app/ezsp-host/ezsp-host-priv.h"
#include "app/ezsp-host/ezsp-host-queues.h"
#include "app/ezsp-host/ezsp-host-ui.h"
#include "app/util/ezsp/serial-interface.h"

//------------------------------------------------------------------------------
// Preprocessor definitions

//------------------------------------------------------------------------------
// Global Variables

EzspStatus ncpError = EZSP_NO_ERROR;                        // ncp error or reset code
EzspStatus hostError = EZSP_NO_ERROR;                        // host error code

bool ncpSleepEnabled;                    // ncp is enabled to sleep

// Config 0 (default) : Mostly dummy values to be compatible with ASH
#define SPI_HOST_CONFIG_DEFAULT                                                   \
  {                                                                               \
    "/dev/ttyS0",       /* serial port name                                  */   \
    115200,             /* baud rate (bits/second)                           */   \
    1,                  /* stop bits                                         */   \
    true,               /* true enables RTS/CTS flow control, false XON/XOFF */   \
    256,                /* max bytes to buffer before writing to serial port */   \
    256,                /* max bytes to read ahead from serial port          */   \
    0,                  /* trace output control bit flags                    */   \
    3,                  /* max frames sent without being ACKed (1-7)         */   \
    false,               /* enables randomizing DATA frame payloads           */  \
    800,                /* adaptive rec'd ACK timeout initial value          */   \
    400,                /*  "     "     "     "     "  minimum value         */   \
    2400,               /*  "     "     "     "     "  maximum value         */   \
    2500,               /* time allowed to receive RSTACK after ncp is reset */   \
    RX_FREE_LWM,        /* if free buffers < limit, host receiver isn't ready */  \
    RX_FREE_HWM,        /* if free buffers > limit, host receiver is ready   */   \
    480,                /* time until a set nFlag must be resent (max 2032)  */   \
    SPI_RESET_METHOD_RST, /* method used to reset ncp                          */ \
    SPI_NCP_TYPE_EM2XX_EM3XX /* type of ncp processor                         */  \
  }

// Host configuration structure
EzspHostConfig spiHostConfig = SPI_HOST_CONFIG_DEFAULT;
