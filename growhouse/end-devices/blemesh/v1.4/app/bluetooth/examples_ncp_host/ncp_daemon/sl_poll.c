/***************************************************************************//**
 * @file
 * @brief sl_poll.c
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

 #include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/param.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include "sl_poll.h"

static struct pollfd polled[SL_POLL_MAX];
static int max_polled = 0;
static int update_polled = 0;

int sl_poll_add(int fd)
{
  if (max_polled >= SL_POLL_MAX) {
    return -1;
  }
  polled[max_polled].fd = fd;
  polled[max_polled].events = POLLIN;
  polled[max_polled].revents = 0;
  max_polled++;
  return 0;
}
void sl_poll_remove(int fd)
{
  int i;
  for (i = 0; i < max_polled; i++) {
    if (polled[i].fd == fd) {
      polled[i].fd = -1;
      update_polled = 1;
      return;
    }
  }
}
int sl_poll_is_polled(int fd)
{
  for (int i = 0; i < max_polled; i++) {
    if (polled[i].fd == fd) {
      return 1;
    }
  }
  return 0;
}
int sl_poll_init()
{
  return 0;
}
int sl_poll_ticks(void*_hw)
{
  struct timeval tv;
  if (gettimeofday(&tv, NULL) != 0) {
    return 0;
  }

  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
int sl_poll_wait(int timeout)
{
  int result;

  //clean table if descriptor was removed
  if (update_polled) {
    int r, w;
    for (r = w = 0; r < max_polled; r++) {
      if (polled[r].fd == -1) {
        continue;
      }
      polled[w] = polled[r];
      w++;
    }
    max_polled = w;
  }

  result = poll(polled, max_polled, timeout == 0 ? -1 : timeout);
  return result;
}
int sl_poll_update()
{
  for (int i = 0; i < max_polled; i++) {
    if (polled[i].revents == 0) {
      continue;
    }

    sl_poll_cb(polled[i].fd, polled[i].revents);
  }

  return 0;
}
