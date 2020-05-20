/***************************************************************************//**
 * @file
 * @brief NCP Daemon Host application for PC. Brdiges UART data to an
 * encrypted and an unencrypted socket.
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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "uart.h"
#include "sl_poll.h"
#include "sl_bgapi.h"
#include "config.h"
#include "sl_security.h"

int enc_socket = -1;
int enc_server_socket = -1;
int unenc_socket = -1;
int unenc_server_socket = -1;
int serial_handle = -1;
sl_bgapi_context_t uart_bgapi;
/** The serial port to use for BGAPI communication. */
static char* uart_port = NULL;
/** The baud rate to use. */
static uint32_t baud_rate = 0;

/** Usage string */
#define USAGE "Usage: %s [serial port] [baud rate] [encrypted_domain_socket] [unencrypted_domain_socket]\n\n"

int uart_init(int argc, char* argv[])
{
  /**
   * Handle the command-line arguments.
   */

  baud_rate = atoi(argv[2]);
  uart_port = argv[1];

  if (!uart_port || !baud_rate ) {
    printf(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }

  /**
   * Initialise the serial port.
   */
  serial_handle = uartOpen((int8_t*)uart_port, baud_rate, 1, -1);
  if (serial_handle < 0) {
    printf("uart open failed\n");
    exit(EXIT_FAILURE);
  }
  sl_poll_add(serial_handle);

  return serial_handle;
}
//
//
int create_domain_socket_server(char *fname)
{
  int len, rc;
  int sock;
  struct sockaddr_un server_sockaddr;
  //create socket
  memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
  sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock == -1) {
    printf("Socket Error: %d\n", errno);
    exit(1);
  }
  server_sockaddr.sun_family = AF_UNIX;
  strcpy(server_sockaddr.sun_path, fname);
  len = sizeof(server_sockaddr);

  //remove existing file
  unlink(fname);
  rc = bind(sock, (struct sockaddr *) &server_sockaddr, len);
  if (rc == -1) {
    printf("BIND ERROR: %d\n", errno);
    close(sock);
    return -1;
  }

  //Listen
  rc = listen(sock, 10);
  if (rc == -1) {
    printf("LISTEN ERROR: %d\n", errno);
    close(sock);
    return -1;
  }
  sl_poll_add(sock);
  return sock;
}
// accept incoming socket, close old socket if exist
int accept_domain_socket(int server, int oldsock)
{
  int n;
  struct sockaddr_un client_sockaddr;
  socklen_t socklen = sizeof(client_sockaddr);
  memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
  // new connection, close previout socket if exist
  if (oldsock >= 0) {
    close(oldsock);
    sl_poll_remove(oldsock);
  }
  // accept connection request
  n = accept(server, (struct sockaddr *) &client_sockaddr, &socklen);
  if (n < 0) {
    return -1;
  }

  return n;
}

/**
 * The main program.
 */
int main(int argc, char* argv[])
{
  int ticks, nticks, tosleep;
  int serialh;
  if (argc < 5) {
    printf(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }
  if (sl_poll_init() < 0) {
    printf("Socket initialization error\n");
    exit(EXIT_FAILURE);
  }
  if ((serialh = uart_init(argc, argv)) < 0) {
    printf("HW init failure\n");
    exit(EXIT_FAILURE);
  }
  if (sl_security_init() < 0) {
    printf("Security init failure\n");
    exit(EXIT_FAILURE);
  }

  enc_server_socket = create_domain_socket_server(argv[3]);
  unenc_server_socket = create_domain_socket_server(argv[4]);
  sl_bgapi_context_init(&uart_bgapi, serialh);
  sl_bgapi_set_ncp_target(&uart_bgapi);
  nticks = ticks = sl_poll_ticks();

  while (1) {
    nticks = sl_poll_ticks();
    tosleep = 0;//ncp_mux_update_timeout(&mux,nticks-ticks);
    ticks = nticks;

    sl_poll_wait(tosleep);

    sl_poll_update();
  }
  return 0;
}
//this callback is called when received bgapi packet needs to be forwarded
void sl_bgapi_process_packet_cb(sl_bgapi_context_t *ctx, char *buf, int len, int encrypted)
{
  if (encrypted) {//if packet is encrypted, forward to encrypted socket
    if (enc_socket >= 0) {
      send(enc_socket, buf, len, 0);
    }
  } else {//unencrypted data, send to both sockets
    if (unenc_socket >= 0) {
      send(unenc_socket, buf, len, 0);
    }
    if (enc_socket >= 0) {
      send(enc_socket, buf, len, 0);
    }
  }
}
//this callback is called when security state changes
void sl_security_state_change_cb(t_sl_security_state state)
{
  switch (state) {
    case sl_security_state_encrypted:
      //move encrypted socket to be polled if not already
      if (!sl_poll_is_polled(enc_socket)) {//
        printf("link encrypted\n");
        sl_poll_add(enc_socket);
      }
      break;
    case sl_security_state_unencrypted:
      //security state is unencrypted
      if (enc_socket >= 0) {
        // move encrypted socket to unpolled state
        if (sl_poll_is_polled(enc_socket)) {//
          printf("link not encrypted\n");
          sl_poll_remove(enc_socket);
        }
        //start security handshake
        sl_security_start();
      }

      break;
    default:
      break;
  }
}

//this callback is called when file descriptor has an event
int sl_poll_cb(int fd, int revents)
{
  char buf[MAX_PACKET_SIZE];

  // encrypted server socket event
  if ((revents & POLLIN) && (fd == enc_server_socket)) {
    enc_socket = accept_domain_socket(enc_server_socket, enc_socket);
    printf("Host connected to encrypted socket\n");
    if (sl_security_state() == sl_security_state_encrypted) {
      //already encrypted, add to poll list
      if (sl_poll_add(enc_socket) < 0) {
        //failed to add to poll list
        close(enc_socket);
      }
    }
    sl_security_start();
  }
  // unencrypted server socket event
  if ((revents & POLLIN) && (fd == unenc_server_socket)) {
    unenc_socket = accept_domain_socket(unenc_server_socket, unenc_socket);
    printf("Host connected to unencrypted socket\n");
    // and add it to poll list
    if (sl_poll_add(unenc_socket) < 0) {
      //failed to add to poll list
      close(unenc_socket);
    }
  }
  // Serial port event
  if ((revents & POLLIN) && (fd == serial_handle)) {
    int len = read(serial_handle, buf, sizeof(buf));
    printf("%d<", len); fflush(stdout);
    sl_bgapi_recv_data(&uart_bgapi, buf, len);
  }
  // Host encrypted socket event
  if ((revents & POLLIN) && (fd == enc_socket)) {
    //receive data after the packet
    int len = recv(fd, buf, sizeof(buf), 0);
    printf("%d>", len); fflush(stdout);

    if (len == 0) {
      //socket closed
      printf("Host encrypted disconnected\n");
      close(fd);
      sl_poll_remove(fd);
      enc_socket = -1;
      return -1;
    } else if (len < 0) {
      return 0;
    } else {
      //ENCRYPT
      char tbuf[MAX_PACKET_SIZE];
      unsigned tlen = len;

      sl_security_encrypt_packet(buf, tbuf, &tlen);

      //
      write(serial_handle, tbuf, (size_t)tlen);
    }
  }
  // Host unencrypted socket event
  if ((revents & POLLIN) && (fd == unenc_socket)) {
    //receive data after the packet
    int len = recv(fd, buf, sizeof(buf), 0);
    printf("%d>", len); fflush(stdout);

    if (len == 0) {
      //socket closed
      printf("Host unencrypted disconnected\n");
      close(fd);
      sl_poll_remove(fd);
      unenc_socket = -1;
      return -1;
    } else if (len < 0) {
      return 0;
    } else {
      write(serial_handle, buf, (size_t)len);
    }
  }
  return 0;
}
