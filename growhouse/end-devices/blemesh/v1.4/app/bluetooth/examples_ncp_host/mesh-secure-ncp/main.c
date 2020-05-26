/***************************************************************************//**
 * @file
 * @brief main.c
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

/**
 * This an example application that demonstrates Bluetooth connectivity
 * using BGLIB commands. These commands are executed in the NCP Target firmware,
 * and sent to the NCP Target through the NCP Daemon over UART in encrypted or
 * unencrypted form. Communication with the NCP Daemon takes place over domain
 * sockets.
 *
 * Most of the functionality in BGAPI uses a request-response-event pattern
 * where the module responds to a command with a command response indicating
 * it has processed the request and then later sending an event indicating
 * the requested operation has been completed. */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

/* BG stack headers */
#include "gecko_bglib.h"

/* application headers */
#include "socket_handler.h"
#include "app.h"

#define CLIENT_ENCRYPTED_PATH "client_encrypted"
#define CLIENT_UNENCRYPTED_PATH "client_unencrypted"

/***************************************************************************************************
 * Local Macros and Definitions
 **************************************************************************************************/

BGLIB_DEFINE();

/** Usage string */
#define USAGE "Usage: %s [server_domain_socket] [is_domain_socket_encrypted? 0/1]\n\n"

/***************************************************************************************************
 * Static Function Declarations
 **************************************************************************************************/

/***************************************************************************************************
 * Public Function Definitions
 **************************************************************************************************/

/***********************************************************************************************//**
 *  \brief  The main program.
 *  \param[in] argc Argument count.
 *  \param[in] argv Buffer contaning Serial Port data.
 *  \return  0 on success, -1 on failure.
 **************************************************************************************************/
int main(int argc, char* argv[])
{
  struct gecko_cmd_packet* evt;

  /* Initialize BGLIB with our output function for sending messages. */
  BGLIB_INITIALIZE_NONBLOCK(onMessageSend, onMessageReceive, messagePeek);

  if (argc < 3) {
    printf(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }

  if (atoi(argv[2])) {
    if (connect_domain_socket_server(argv[1], CLIENT_ENCRYPTED_PATH, 1)) {
      printf("Connection to encrypted domain socket unsuccessful. Exiting..\n");
      exit(EXIT_FAILURE);
    }
    printf("Turning on Encryption. All subsequent BGAPI commands and events will be encrypted..\n");
    turnEncryptionOn();
  } else {
    if (connect_domain_socket_server(argv[1], CLIENT_UNENCRYPTED_PATH, 0)) {
      printf("Connection to unencrypted domain socket unsuccessful. Exiting..\n");
      exit(EXIT_FAILURE);
    }
  }

  printf("Starting up...\nResetting NCP target...\n");

  /* Reset NCP to ensure it gets into a defined state.
   * Once the chip successfully boots, gecko_evt_system_boot_id event should be received. */
  gecko_cmd_system_reset(0);

  while (1) {
    poll_update(50);

    /* Check for stack event. */
    evt = gecko_peek_event();
    /* Run application and event handler. */
    appHandleEvents(evt);
  }

  return -1;
}
