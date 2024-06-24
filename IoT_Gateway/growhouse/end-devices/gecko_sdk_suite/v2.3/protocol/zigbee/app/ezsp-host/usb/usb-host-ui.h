/******************************************************************************/
/**
 * @file usb-host-ui.h
 * @brief Additional header for EZSP USB Host user interface functions
 *
 * See @ref usb_util for documentation.
 *
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
/** @addtogroup usb_util
 *
 * See usb-host-ui.h.
 *
 *@{
 */

#ifndef SILABS_USB_HOST_UI_H
#define SILABS_USB_HOST_UI_H

static const char usage[] =
  " {options}\n"
  "  options:\n"
  "    -h                display usage information\n"
  "    -p <port>         serial port name or number (eg, COM1, ttyS0, or 1)\n"
  "    -t <trace flags>  trace B0=frames, B1=verbose frames, B2=events, B3=EZSP\n";

bool ezspInternalProcessCommandOptions(int argc, char *argv[], char *errStr);
#endif //__USB_HOST_UI_H___

/** @} END addtogroup
 */
