/***************************************************************************//**
 * @file
 * @brief sl_poll.h
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

 #ifndef SL_POLL_H
#define SL_POLL_H

//max number of descriptors to poll
#define SL_POLL_MAX 10

int sl_poll_init();
int sl_poll_ticks();
int sl_poll_wait(int timeout);
int sl_poll_update();
int sl_poll_add(int fd);
void sl_poll_remove(int fd);
int sl_poll_cb(int fd, int revents);
int sl_poll_is_polled(int fd);

#endif
