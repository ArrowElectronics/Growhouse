/******************************************************************************/
/**
 * @file usb-host.c
 * @brief EZSP USB Host user interface functions
 *
 * This includes command option parsing, trace and counter functions.
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
#include PLATFORM_HEADER
#include <string.h>
#ifndef WIN32
  #include <unistd.h>
  #include <time.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
#endif
#include <stdlib.h>
#include "stack/include/ember-types.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/ezsp-host/ezsp-host-common.h"
#include "app/ezsp-host/usb/usb-host.h"
#include "app/ezsp-host/ezsp-host-priv.h"
#include "app/ezsp-host/ezsp-host-io.h"
#include "app/ezsp-host/ezsp-host-queues.h"
#include "app/ezsp-host/ezsp-host-ui.h"
#include "app/util/gateway/backchannel.h"
#include "app/ezsp-host/usb/usb-host-ui.h"

//------------------------------------------------------------------------------
// Preprocessor definitions

#define ERR_LEN               128   // max length error message

#define txControl (txBuffer[0])     // more descriptive aliases
#define rxControl (rxBuffer[0])

static const char options[] = "h::n:p:t:";
bool checkSerialPort(const char* portString, bool silent);

bool ezspInternalProcessCommandOptions(int argc, char *argv[], char *errStr)
{
  int c;
  char port[USB_PORT_LEN];
  char devport[USB_PORT_LEN];
  uint8_t trace;
  uint8_t portnum;
  int optionCount = 0;

  while (true) {
    c = getopt(argc, argv, options);
    if (c == -1) {
      if (optind != argc ) {
        snprintf(errStr, ERR_LEN, "Invalid option %s.\n", argv[optind]);
      }
      break;
    }

    optionCount++;

    switch (c) {
      case 'h':
      case '?':
        snprintf(errStr, ERR_LEN, "\n");
        break;
      case 'p':
        sscanf(optarg, "%39s", port);
        if (strlen(port) >= USB_PORT_LEN - 1) {
          snprintf(errStr, ERR_LEN, "Serial port name %s too long.\n", port);
        } else {
          // Handle some common variations specifying a serial port
          strcpy(devport, "/dev/");
          if ( strncmp(devport, port, 5) == 0) {
            strcat(devport, port + 5);
  #ifdef __CYGWIN__
          } else if ( ((strncmp("COM", port, 3) == 0)
                       || (strncmp("com", port, 3) == 0) )
                      && (sscanf(port + 3, "%hhu", &portnum) == 1)
                      && (portnum > 0) ) {
            sprintf(devport, "/dev/ttyS%hhu", portnum - 1);
          } else if ( (sscanf(port, "%hhu", &portnum) == 1) && portnum ) {
            sprintf(devport, "/dev/ttyS%hhu", portnum - 1);
  #else
          } else if (sscanf(port, "%hhu", &portnum) == 1) {
            sprintf(devport, "/dev/ttyS%hhu", portnum);
  #endif
          } else {
            strcat(devport, port);
          }
          strncpy(usbHostConfig.serialPort, devport, USB_PORT_LEN);
          if (!checkSerialPort(usbHostConfig.serialPort, false)) {
            return false;
          }
        }
        break;
      case 't':
        if (sscanf(optarg, "%hhu", &trace) != 1) {
          snprintf(errStr, ERR_LEN, "Invalid trace flag value %s.\n", optarg);
        } else {
          usbWriteConfig(traceFlags, trace);
        }
        break;
      default:
        assert(1);
        break;
    }   // end of switch (c)
  } //end while
  return true;
}
